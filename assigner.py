#As we need to read 2 files as input, writing a function to reuse code
def read_file(x):
    fh=open(x,"r")
    data_as_text=fh.readlines()
    data=[]
    for i in range(0,len(data_as_text)):
        data.append(data_as_text[i].split(","))
    for i in range(0,len(data)):
        for j in range(0,len(data[i])):
            try:
                data[i][j]=int(data[i][j])
            except:
                continue
    return data
def str_of_2Darray(x):
    output=""
    for i in range(0,len(x)):
        line=""
        for j in range(0,len(x[i])):
            line=line+str(x[i][j])+","
        line=line[:-1]
        line=line+"\n"
        output=output+line
    output=output[:-1]
    return output
def print_array_in_file(x,y):
    fh=open(y,"w")
    str_of_x=str_of_2Darray(x)
    fh.write(str_of_x)
    fh.close()
#reading files
def pruning(x):
    for i in range(len(x)):
        if ("\r\n" in x[i][-1]):
            x[i]=x[i][:-1]
    for i in range(len(x)):
        for j in range(len(x[i])):
            if (x[i][j]==''or x[i][j]==' '):
                x[i]=x[i][:j]
                break
    return x
temp_student_data=read_file("Students.csv")
student_data=pruning(temp_student_data)
student_data.sort()
course_data=read_file("Courses.csv")
course_data.sort()
#these are going to be of use
number_of_courses=len(course_data)
number_of_students=len(student_data)
#list of sorted course IDs
courseID=[]
course_credit=[]
for i in range(0,number_of_courses):
    courseID.append(course_data[i][0])
    course_credit.append(20)
courseID.append("Dissertation")
course_credit.append(40)
courseID.append("Dissertation")
course_credit.append(40)
#modify student data a bit
for i in range(0,number_of_students):
    for j in range(5,len(student_data[i])):
        student_data[i][j]=courseID.index(student_data[i][j])
# will declare arrays with same name as used in problem model
#Quota for each course
q=[]
#Whether pre-1800s or not
p=[]
#When is the course scheduled to take place
t=[]
for i in range(0,number_of_courses):
    q.append(course_data[i][3])
    if(course_data[i][2]=="Y"):
        #if course is pre-1800s, indicate it by 1
        p.append(1)
    else:
        #if course id not pre-1800s, indicate it by 0
        p.append(0)
    if(course_data[i][1]==1):
        # if course takes place in 1st sem, indicate by 0
        t.append(0)
    else:
        #if course takes place in 2nd sem, indicate by 1
        t.append(1)
t.append(0)
t.append(1)
#number of courses each student has to take
b=[]
#number of pre-1800s courses each student has to take
z=[]
for i in range(0,number_of_students):
    #this information is given and we directly append it.
    if(student_data[i][1]==3):
        z.append(0)
        if(student_data[i][2]=="S"):
            b.append(4)
        else:
            b.append(2)
    else:
        z.append(student_data[i][3])
        if(student_data[i][2]=="S"):
            b.append(4)
        else:
            if(student_data[i][4]=="0"):
                b.append(3)
            else:
                b.append(1)
#preference matrix
preference=[]
for i in range(0,number_of_students):
    preference.append(student_data[i][5:])
#rank matrix
r=[]
for i in range(0, number_of_students):
    line = []
    for j in range(0, number_of_courses):
        line.append(0)
    r.append(line)
for i in range(0, number_of_students):
    for j in range(0, len(preference[i])):
        r[i][preference[i][j]] = j
    
#----------------Input has been formatted-------------------------
import pulp

prob = pulp.LpProblem('Course Assignment', pulp.LpMinimize)

#declaring the X matrix
X = [[pulp.LpVariable("x%dx%d" % (i, j), cat='Binary')for j in range(number_of_courses + 2)] for i in range(number_of_students)]
#dummy variables.The eighteen themselves had their eyes wide open. They could see the disaster coming as well with the finger strikes penetrating their sk
#in case givng someone more courses makes life simpler
maxStd = [pulp.LpVariable("maxStd%d" % (i), lowBound = 0, cat='Integer') for i in range(number_of_students)]
# in case someone doing less pre-1800s courses make life simpler
preStd = [pulp.LpVariable("preStd%d" % (i), lowBound = 0, cat='Integer') for i in range(number_of_students)]
# if overfilling course makes life simpler
oveCor = [pulp.LpVariable("oveCor%d" % (i), lowBound = 0, cat='Integer') for i in range(number_of_courses)]
# if having less students in a course makes life simpler
minCor = [pulp.LpVariable("minCor%d" % (i), lowBound = 0, cat='Integer') for i in range(number_of_courses)]
# If first semester credits with dissertation mess up
firDis = [pulp.LpVariable("firDis%d" % (i), lowBound = 0, cat='Integer') for i in range(number_of_students)]
# If second semester credits witht disseration mess up
secDis = [pulp.LpVariable("secDis%d" % (i), lowBound = 0, cat='Integer') for i in range(number_of_students)]
#First semester load messing up
firStd = [pulp.LpVariable("firStd%d" % (i), lowBound = 0, cat='Integer') for i in range(number_of_students)]
#Second semester load messing up
secStd = [pulp.LpVariable("secStd%d" % (i), lowBound = 0, cat='Integer') for i in range(number_of_students)]
#Values for objective function
n_3=0
n_4=0
max_len=0
for i in range(number_of_students):
    if(student_data[i][1]==3):
        n_3=n_3+1
    if(student_data[i][1]==4):
        n_4=n_4+1
    if(len(student_data[i])-5 > max_len):
        max_len=len(student_data[i])-5
#L3 penalty
scale=0
for i in range(6):
    scale=scale+(max_len-i)
scale_junior=(max_len+1)*n_4
scale_quota=scale*n_4*(n_3+1)*(scale+1)
scale_cir=scale_quota*number_of_courses
#objective function(need to change)
prob.setObjective(sum(sum((X[i][j]*r[i][j])/b[i] for j in range(number_of_courses))*(4-student_data[i][1]) for i in range(number_of_students))
 + ((max_len+1)*(student_data[i][1]-3)*(X[i][number_of_courses+1]*(2-student_data[i][4])+(student_data[i][4]-1)*X[i][number_of_courses])for i in range(number_of_students))
 + sum(sum((X[i][j]*r[i][j])/b[i] for j in range(number_of_courses))* scale_junior * (student_data[i][1]-3) for i in range(number_of_students))
 + (scale_quota * sum(oveCor[i] + minCor[i] for i in range(number_of_courses)))
 + (scale_cir * sum(maxStd[i] + preStd[i] + firDis + secDis + firStd[i] + secStd[i] for i in range(number_of_students)))
)
#general constraints for each student
for i in range(0,number_of_students):
#constraint about number of courses needed
    prob.addConstraint(sum(X[i][j] for j in range(0,number_of_courses))+maxStd[i] ==b[i])
#constraint about pre-1800s courses
    prob.addConstraint(sum(X[i][j]*p[j] for j in range(0,number_of_courses))+preStd[i]>=z[i])
#going to say if a number is not in preference list just give it 0
for i in range(0,number_of_students):
    for j in range(0, number_of_courses):
        if(j not in preference[i]):
#can't get courses not in preference list
            prob.addConstraint(X[i][j]==0)
#genereal constraint for each course
for i in range(0,number_of_courses):
#quota constraint for each course
    prob.addConstraint(sum(X[j][i] for j in range(0,number_of_students))-oveCor[i]<=q[i])
#minimum student taking constraint for english lit courses
    if("ENGLIT" in course_data[i][0]):
        prob.addConstraint(sum(X[j][i] for j in range(0,number_of_students))+minCor[i]>=8)
for i in range(0,number_of_students):
    #for 3rd year single students
    if(student_data[i][1]==3 and student_data[i][2]=="S"):
        #dissertation constraint
        prob.addConstraint(X[i][number_of_courses] + X[i][number_of_courses + 1] == 0)
        #load equalizer constraint
        prob.addConstraint(sum(t[j]*X[i][j] for j in range(0, number_of_courses))-secStd[i]== 2)
        #load equalizer constraint
        prob.addConstraint(sum((1-t[j])*X[i][j] for j in range(0, number_of_courses))-firStd[i]== 2)
        continue
    #for 3rd year joint students
    elif(student_data[i][1]==3 and student_data[i][2]=="J"):
        #dissertation constraint
        prob.addConstraint(X[i][number_of_courses] + X[i][number_of_courses + 1] == 0)
        if (student_data[i][4]==1):
            #load equalizer constraint
            prob.addConstraint(sum(t[j]*X[i][j] for j in range(0, number_of_courses))-secStd[i]<=2)
            #load equalizer constraint
            prob.addConstraint(sum((1-t[j])*X[i][j] for j in range(0, number_of_courses))-firStd[i]<=1)
        else:
            #load equalizer constraint
            prob.addConstraint(sum(t[j]*X[i][j] for j in range(0, number_of_courses))-secStd[i]<=1)
            #load equalizer constraint
            prob.addConstraint(sum((1-t[j])*X[i][j] for j in range(0, number_of_courses))-firStd[i]<=2)
        continue
    #for 4th year single students
    elif(student_data[i][1]==4 and student_data[i][2]=="S"):
        #dissertation constraint
        prob.addConstraint(X[i][number_of_courses] + X[i][number_of_courses + 1] == 1)
        #credit constraint
        prob.addConstraint(sum(X[i][j] for j in range(0, number_of_courses))==4)
        #load equalizer constraint
        prob.addConstraint(sum((1-t[j])*X[i][j] for j in range(0, number_of_courses))+(number_of_courses*X[i][number_of_courses]) - firDis[i] <=number_of_courses+1)
        #load equalizer constraint
        prob.addConstraint(sum(t[j]*X[i][j] for j in range(0, number_of_courses))+(number_of_courses*X[i][number_of_courses+1]) - secDis[i] <=number_of_courses+1)
        #load equalizer constraint
        prob.addConstraint(sum(t[j]*X[i][j] for j in range(0, number_of_courses))-secStd[i]<=3)
        #load equalizer constraint
        prob.addConstraint(sum((1-t[j])*X[i][j] for j in range(0, number_of_courses))-firStd[i]<=3)
        continue
    #for 4th year joint students
    else:
        #dissertation constraint
        prob.addConstraint(X[i][number_of_courses] + X[i][number_of_courses + 1] <= 1)
        #credit constraint
        prob.addConstraint(sum(X[i][j] for j in range(0, number_of_courses+2))+(X[i][number_of_courses]+X[i][number_of_courses+1])==3)
        #load equalizer constraint
        prob.addConstraint(sum((1-t[j])*X[i][j] for j in range(0, number_of_courses))+(number_of_courses*X[i][number_of_courses]) - firDis[i] <=number_of_courses)
        #load equalizer constraint
        prob.addConstraint(sum(t[j]*X[i][j] for j in range(0, number_of_courses))+(number_of_courses*X[i][number_of_courses+1]) - secDis[i] <=number_of_courses)
        #load equalizer constraint Feasible solution hasn't been found (but may exist). 
        prob.addConstraint(sum(t[j]*X[i][j] for j in range(0, number_of_courses))- secStd[i]<= 2)
        #load equalizer constraint
        prob.addConstraint(sum((1-t[j])*X[i][j] for j in range(0, number_of_courses))-firStd[i]<= 2)
        continue
#solving
prob.solve()
print "\n", "Status:", pulp.LpStatus[prob.status], "\n"
print ("Minimum=", pulp.value(prob.objective))

summation=0
last_summation=0
#can't use X matrix as it is a pulp object
final_matrix=[]
for i in range(0,number_of_students):
    line=[]
    for j in range(0,number_of_courses+2):
        line.append(int(X[i][j].value()))
    final_matrix.append(line)

# suppose we got our matrix.
student_output=[]
for i in range(0, number_of_students):
    line=[]
    credit=0
    temp=[]
    line.append(student_data[i][0])
    line.append(student_data[i][1])
    line.append(student_data[i][2])
    line.append("Semester 1")
    for j in range(number_of_courses+2):
        if(final_matrix[i][j]==1 and t[j]==0):
            temp.append(courseID[j])
            credit=credit+course_credit[j]
    line.append(credit)
    line=line+temp
    student_output.append(line)
    line=[]
    credit=0
    temp=[]
    line.append(student_data[i][0])
    line.append(student_data[i][1])
    line.append(student_data[i][2])
    line.append("Semester 2")
    for j in range(number_of_courses+2):
        if(final_matrix[i][j]==1 and t[j]==1):
            temp.append(courseID[j])
            credit=credit+course_credit[j]
    line.append(credit)
    line=line+temp
    student_output.append(line)
print_array_in_file(student_output,"Student-output.csv")

course_output=[]
for i in range(0, number_of_courses):
    line=[]
    line.append(courseID[i])
    line.append("Semester"+str(t[i]+1))
    if(p[i]==1):
        line.append("Pre-1800s course")
    else:
        line.append("Not a Pre-1800s course")
    line.append(q[i])
    summation=0
    for j in range(number_of_students):
        summation=summation+final_matrix[j][i]
    line.append(summation)
    for j in range(number_of_students):
        if(final_matrix[j][i]==1):
            line.append(student_data[j][0])
    course_output.append(line)
final_course_output=[["Course ID", "Timing of the Course", "Whether pre-1800s or not", "Capacity", "Number Assigned", "Students assigned"]]
final_course_output=final_course_output+course_output
print_array_in_file(final_course_output,"Course-output.csv")


admin_output=[]
#Overall Cost for all students
admin_output.append(["The overall cost for students i.e. the sums of ranks of courses they are assigned"])
cost=0
for i in range(number_of_students):
    for j in range(number_of_courses):
        cost=cost+(final_matrix[i][j]*r[i][j])
admin_output.append([cost])

#Overall Profile
admin_output.append(["Profile of the students i.e. Number of people who got their ith choice"])
profile=[]
for i in range(number_of_courses):
    profile.append(0)
for i in range(number_of_students):
    for j in range(number_of_courses):
        if(final_matrix[i][j]==1):
            profile[r[i][j]]=profile[r[i][j]]+1
admin_output.append(profile)
#average cost of students of each type
admin_output.append(["The average cost for each type of student i.e. the sums of ranks of courses they are assigned"])
admin_output.append(["3rd year Single", "3rd year Joint", "4th year Single", "4th year Joint"])
profile=[[0,0],[0,0],[0,0],[0,0]]
for i in range(number_of_students):
    for j in range(number_of_courses):
        if(final_matrix[i][j]==1):
            if(student_data[i][1]==3 and student_data[i][2]=="S"):
                profile[0][0]=profile[0][0]+r[i][j]+1
                profile[0][1]=profile[0][1]+1
            elif(student_data[i][1]==3 and student_data[i][2]=="J"):
                profile[1][0]=profile[1][0]+r[i][j]+1
                profile[1][1]=profile[1][1]+1
            elif(student_data[i][1]==4 and student_data[i][2]=="S"):
                profile[2][0]=profile[2][0]+r[i][j]+1
                profile[2][1]=profile[2][1]+1
            else:
                profile[3][0]=profile[3][0]+r[i][j]+1
                profile[3][1]=profile[3][1]+1
profile[0]=float(profile[0][0])/(profile[0][1])
profile[1]=float(profile[1][0])/(profile[1][1])
profile[2]=float(profile[2][0])/(profile[2][1])
profile[3]=float(profile[3][0])/(profile[3][1])
admin_output.append(profile)
#Profile for each type of student
admin_output.append(["Student type-wise profile i.e. Number of people who got their ith choice"])
profile=[[],[],[],[]]
for i in range(number_of_courses):
    profile[0].append(0)
    profile[1].append(0)
    profile[2].append(0)
    profile[3].append(0)
for i in range(number_of_students):
    for j in range(number_of_courses):
        if(final_matrix[i][j]==1):
            if(student_data[i][1]==3 and student_data[i][2]=="S"):
                profile[0][r[i][j]]=profile[0][r[i][j]]+1
            elif(student_data[i][1]==3 and student_data[i][2]=="J"):
                profile[1][r[i][j]]=profile[1][r[i][j]]+1
            elif(student_data[i][1]==4 and student_data[i][2]=="S"):
                profile[2][r[i][j]]=profile[2][r[i][j]]+1
            else:
                profile[3][r[i][j]]=profile[3][r[i][j]]+1
admin_output.append(["3rd year Single"])
admin_output.append(profile[0])
admin_output.append(["3rd year Joint"])
admin_output.append(profile[1])
admin_output.append(["4th year Single"])
admin_output.append(profile[2])
admin_output.append(["4th year Joint"])
admin_output.append(profile[3])

min_attendance=number_of_students
max_attendance=0
for i in range(number_of_courses):
    if(course_output[i][4]>max_attendance):
        max_attendance=course_output[i][4]
    if(course_output[i][4]<min_attendance):
        min_attendance=course_output[i][4]
line_min=[min_attendance]
line_max=[max_attendance]
for i in range(number_of_courses):
    if(course_output[i][4]==max_attendance):
        line_max.append(course_output[i][0])
    if(course_output[i][4]==min_attendance):
        line_min.append(course_output[i][0])
admin_output.append(["Minimum attendance", "Courses"])
admin_output.append(line_min)
admin_output.append(["Maximum Attendance", "Courses"])
admin_output.append(line_max)
##############Checker##################
admin_output.append([])
admin_output.append([])
admin_output.append(["The following course constraints were violated:-"])
#checker for Course Qouta 
for j in range(number_of_courses):
    output_line = ""
    if(sum(final_matrix[i][j] for i in range(0,number_of_students)) > q[j]):
        output_line = "Course quota exceeded for course " + courseID[j]
    if(output_line != ""):
        admin_output.append([output_line])
#checker for Not enough students in a course
for j in range(number_of_courses):
	if("ENGLIT" in course_data[i][0] ):
		output_line = ""
		if(sum(final_matrix[i][j] for i in range(0,number_of_students)) < 8):
		    output_line = "Not enough students in course " + courseID[j]
		if(output_line != ""):
		    admin_output.append([output_line])
admin_output.append([])
admin_output.append([])
admin_output.append(["The following Student constraints were violated:-"])
for i in range(number_of_students):
    output_line="For Student " + str(student_data[i][0]) + " the following constraints were violated:-\n"
    if(sum(final_matrix[i][j] for j in range(number_of_courses)) < b[i]):
        output_line = output_line + "Not enough courses assigned \n"
    if(sum(final_matrix[i][j] * p[j] for j in range(number_of_courses)) < z[i]):
        output_line = output_line + "Not enough pre-1800s courses assigned\n"
    if(student_data[i][1]==3 and student_data[i][2]=="S"):
        #dissertation constraint
        if(final_matrix[i][number_of_courses] + final_matrix[i][number_of_courses + 1] != 0):
            output_line = output_line + "3rd year Single student took Dissertation \n"
        #load equalizer constraint
        if(sum(t[j]*X[i][j] for j in range(0, number_of_courses))!=2):
            output_line = output_line + "3rd year Single student took too many courses in the second Semester\n"
        #load equalizer constraint
        if(sum((1-t[j])*X[i][j] for j in range(0, number_of_courses))!=2):
            output_line = output_line + "3rd year Single student took too many courses in the first Semester\n"
    #for 3rd year joint students
    elif(student_data[i][1]==3 and student_data[i][2]=="J"):
        #dissertation constraint
        if(final_matrix[i][number_of_courses] + final_matrix[i][number_of_courses + 1] != 0):
            output_line = output_line + "3rd year Joint student took Dissertation\n"
        #load equalizer constraint
        if(student_data[i][4]==1):
            if(sum(t[j]*final_matrix[i][j] for j in range(0, number_of_courses))>2):
                output_line = output_line + "3rd year Joint student took too many courses in the second semester\n"
            if(sum(t[j]*final_matrix[i][j] for j in range(0, number_of_courses))==0):
                output_line = output_line + "3rd year Joint student took 0 courses in the second semester\n"
        else:
            if(sum(t[j]*final_matrix[i][j] for j in range(0, number_of_courses))>1):
                output_line = output_line + "3rd year Joint student took too many courses in the second semester\n"
            #load equalizer constraint
            if(sum((1-t[j])*final_matrix[i][j] for j in range(0, number_of_courses))>2):
                output_line = output_line + "3rd year Joint student took too many courses in the first semester\n"
        #for 4th year single students
    elif(student_data[i][1]==4 and student_data[i][2]=="S"):
        #dissertation constraint
        if(final_matrix[i][number_of_courses] + final_matrix[i][number_of_courses + 1] != 1):
            output_line = output_line + "4th year Single student did not take Dissertation\n"
        #credit constraint
        if(sum(final_matrix[i][j] for j in range(0, number_of_courses))!=4):
            output_line = output_line + "4th year Single student did not take enough credits\n"
        #load equalizer constraint
        if(sum((1-t[j])*final_matrix[i][j] for j in range(0, number_of_courses))+(number_of_courses*final_matrix[i][number_of_courses])>number_of_courses+1):
            output_line = output_line + "4th year Single student took too many courses with Dissertation both in semester 1\n"
        #load equalizer constraint
        if(sum(t[j]*final_matrix[i][j] for j in range(0, number_of_courses))+(number_of_courses*final_matrix[i][number_of_courses+1])>number_of_courses+1):
            output_line = output_line + "4th year Single student took too many courses with Dissertation both in semester 2\n"
        #load equalizer constraint
        if(sum(t[j]*final_matrix[i][j] for j in range(0, number_of_courses))>3):
            output_line = output_line + "4th year Single student took too many courses in semester 2\n"
        #load equalizer constraint
        if(sum((1-t[j])*final_matrix[i][j] for j in range(0, number_of_courses))>3):
            output_line = output_line + "4th year Single student took too many courses in semester 1\n"
        #for 4th year joint students
    else:
        #dissertation constraint
        if(final_matrix[i][number_of_courses] + final_matrix[i][number_of_courses + 1] > 1):
            output_line = output_line + "4th year Joint student took multiple Dissertations\n"
        #credit constraint
        if(sum(final_matrix[i][j] for j in range(0, number_of_courses+2))+(final_matrix[i][number_of_courses]+final_matrix[i][number_of_courses+1])!=3):
            output_line = output_line + "4th year Joint student did not take enough credits\n"
        #load equalizer constraint
        if(sum((1-t[j])*final_matrix[i][j] for j in range(0, number_of_courses))+(number_of_courses*final_matrix[i][number_of_courses])>number_of_courses):
            output_line = output_line + "4th year Joint student took too many courses with Dissertation both in semester 1\n"
        #load equalizer constraint
        if(sum(t[j]*final_matrix[i][j] for j in range(0, number_of_courses))+(number_of_courses*final_matrix[i][number_of_courses+1])>number_of_courses):
            output_line = output_line + "4th year Joint student took too many courses with Dissertation both in semester 2\n"
        #load equalizer constraint
        if(sum(t[j]*final_matrix[i][j] for j in range(0, number_of_courses))>2):
            output_line = output_line + "4th year Joint student took too many courses in semester 2\n"
        #load equalizer constraint
        if(sum((1-t[j])*final_matrix[i][j] for j in range(0, number_of_courses))>2):
            output_line = output_line + "4th year Joint student took too many courses in semester 1\n"
    if(output_line != "For Student " + str(student_data[i][0]) + " the following constraints were violated:-\n"):
        admin_output.append([output_line])
print_array_in_file(admin_output,"Admin-output.txt")
#######################################################DEBUGGING#########################################
