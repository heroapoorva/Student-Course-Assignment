#need random package to generate random test cases
import random
#number of students
no_std = 190
#number of courses
no_cor = 20

#A function to convert 2d arrays to a single string and write it to a file
def str_of_2Darray(array):
    #converts 2d array to a single string
    output = ""
    for i in range(0, len(array)):
        line = ""
        for j in range(0, len(array[i])):
            line = line+str(array[i][j]) + ","
        line = line[:-1]
        line = line + "\n"
        output = output + line
    output = output[:-1]
    return output

def print_array_in_file(array, name):
    #writes a string to file
    fh = open(name, "w")
    str_of_array = str_of_2Darray(array)
    fh.write(str_of_array)
    fh.close()
#We start by assigning 2 courses to each of the category
start = range(8)
#first semester and pre-1800s
pre1 = start[:2]
#first semester and not pre-1800s
npre1 = start[2:4]
#second semester and pre-1800s
pre2 = start[4:6]
#second semester and not pre-1800s
npre2 = start[6:]
output = []
#adding details for each of the 8 courses
for i in range(8):
    line = []
    #course ID
    line.append(i)
    if(i in pre1):
        #Semester course is offered in
        line.append(1)
        #pre-1800s or not
        line.append("Y")
    elif(i in pre2):
        #Semester course is offered in
        line.append(2)
        #pre-1800s or not
        line.append("Y")
    elif(i in npre1):
        #Semester course is offered in
        line.append(1)
        #pre-1800s or not
        line.append("N")
    else:
        #Semester course is offered in
        line.append(2)
        #pre-1800s or not
        line.append("N")
    #course Quota
    line.append(random.randint(8, 60))
    output.append(line)
for i in range(8, no_cor):
    line = []
    #course ID
    line.append(i)
    #Semester course if offered in
    line.append(random.randint(1, 2))
    #Pre-1800s or not
    line.append(random.choice("NY"))
    #Course Quota
    line.append(random.randint(8, 60))
    #appending course to the category list
    if(line[1] == 1):
        if(line[2] == "N"):
            npre1.append(i)
        else:
            pre1.append(i)
    else:
        if(line[2] == "N"):
            npre2.append(i)
        else:
            pre2.append(i)
    output.append(line)
random.shuffle(output)
print_array_in_file(output, "Courses.csv")


output = []
for i in range(no_std):
    line = []
    course_list = []
    #Student ID
    line.append(i)
    #3rd year or 4th year
    line.append(random.randint(3, 4))
    #Single or joint
    line.append(random.choice("SJ"))
    if(line[1] == 3):
        #number of required pre-1800s course
        line.append(0)
        if(line[2] == "S"):
            #Course/Dissertation Preference
            line.append(-1)
            #Course list for the student
            course_list = course_list + random.sample(pre1+npre1, 4)
            course_list = course_list + random.sample(pre2+npre2, 4)
        else:
            #Course/Dissertation Preference
            line.append(random.randint(1, 2))
            #Course list for the student
            course_list = course_list + random.sample(pre1+npre1, 4)
            course_list = course_list + random.sample(pre2+npre2, 4)
    else:
        if(line[2] == "S"):
            #number of required pre-1800s course
            line.append(random.randint(0, 2))
            #Course/Dissertation Preference
            line.append(random.randint(0, 2))
            #Course list for the student
            course_list = course_list + random.sample(pre1, 2)
            course_list = course_list + random.sample(pre2, 2)
            course_list = course_list + random.sample(pre1+npre1, 6)
            course_list = course_list + random.sample(pre2+npre2, 6)
        else:
            #number of required pre-1800s course
            line.append(random.randint(0, 1))
            #Course/Dissertation Preference
            line.append(random.randint(-1, 2))
            if(line[-1] == -1):
                #Course list for the student
                course_list = course_list + random.sample(pre1, 2)
                course_list = course_list + random.sample(pre2, 2)
                course_list = course_list + random.sample(pre1+npre1, 6)
                course_list = course_list + random.sample(pre2+npre2, 6)
            else:
                #Course list for the student
                course_list = course_list + random.sample(pre1, 2)
                course_list = course_list + random.sample(pre2, 2)
                course_list = course_list + random.sample(pre1+npre1, 2)
                course_list = course_list + random.sample(pre2+npre2, 2)
    course_list = list(set(course_list))
    random.shuffle(course_list)
    line = line + course_list
    output.append(line)
random.shuffle(output)
print_array_in_file(output, "Students.csv")
