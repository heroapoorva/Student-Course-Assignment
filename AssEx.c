#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h> 
#include <stdbool.h> 

// global variables
enum {LCS, ED, SW, NONE} alg_type; // which algorithm to run
char *alg_desc; // description of which algorithm to run
char *result_string; // text to print along with result from algorithm
char *x, *y; // the two strings that the algorithm will execute on
char *filename; // file containing the two strings
int xLen, yLen, alphabetSize; // lengths of two strings and size of alphabet
bool iterBool = false, recNoMemoBool = false, recMemoBool = false; // which type of dynamic programming to run
bool printBool = false; // whether to print table
bool readFileBool = false, genStringsBool = false; // whether to read in strings from file or generate strings randomly
// functions follow
long long int count=0;
// determine whether a given string consists only of numerical digits
bool isNum(char s[]) {
	int i;
	bool isDigit=true;
	for (i=0; i<strlen(s); i++)
		isDigit &= s[i]>='0' && s[i]<='9';
	return isDigit;
}

// get arguments from command line and check for validity (return true if and only if arguments illegal)
bool getArgs(int argc, char *argv[]) {
	int i;
	alg_type = NONE;
	xLen = 0;
	yLen = 0;
	alphabetSize = 0;
	for (i = 1; i < argc; i++) // iterate over all arguments provided (argument 0 is name of this module)
		if (strcmp(argv[i],"-g")==0) { // generate strings randomly
			if (argc>=i+4 && isNum(argv[i+1]) && isNum(argv[i+2]) && isNum(argv[i+3])) { // must be three numerical arguments after this
				xLen=atoi(argv[i+1]); // get length of x
				yLen=atoi(argv[i+2]); // get length of y
				alphabetSize = atoi(argv[i+3]); // get alphabet size
				genStringsBool = true; // set flag to generate strings randomly
				i+=3; // ready for next argument
			}
			else
				return true; // must have been an error with -g arguments
		}
		else if (strcmp(argv[i],"-f")==0) { // read in strings from file
			if (argc>=i+2) { // must be one more argument (filename) after this)
				i++;
				filename = argv[i]; // get filename
				readFileBool = true; // set flag to read in strings from file 
			}
			else
				return true; // must have been an error with -f argument 
		}
		else if (strcmp(argv[i],"-i")==0) // iterative dynamic programming
			iterBool = true;
		else if (strcmp(argv[i],"-r")==0) // recursive dynamic programming without memoisation
			recNoMemoBool = true;
		else if (strcmp(argv[i],"-m")==0) // recursive dynamic programming with memoisation
			recMemoBool = true;
		else if (strcmp(argv[i],"-p")==0) // print dynamic programming table
			printBool = true;
		else if (strcmp(argv[i],"-t")==0) // which algorithm to run
			if (argc>=i+2) { // must be one more argument ("LCS" or "ED" or "SW")
				i++;
				if (strcmp(argv[i],"LCS")==0) { // Longest Common Subsequence
					alg_type = LCS;
					alg_desc = "Longest Common Subsequence";
					result_string = "Length of a longest common subsequence is";
				}
				else if (strcmp(argv[i],"ED")==0) { // Edit Distance
					alg_type = ED;
					alg_desc = "Edit Distance";
					result_string = "Edit distance is";
				}
				else if (strcmp(argv[i],"SW")==0) { // Smith-Waterman Algorithm
					alg_type = SW;
					alg_desc = "Smith-Waterman algorithm";
					result_string = "Length of a highest scoring local similarity is";
				}
				else
					return true; // none of these; illegal choice
			}
			else
				return true; // algorithm type not given
		else
			return true; // argument not recognised
		// check for legal combination of choices; return true (illegal) if user chooses:
		// - neither or both of generate strings and read strings from file
		// - generate strings with length 0 or alphabet size 0
		// - no algorithm to run
		// - no type of dynamic programming
		return !(readFileBool ^ genStringsBool) || (genStringsBool && (xLen <=0 || yLen <= 0 || alphabetSize <=0)) || alg_type==NONE || (!iterBool && !recMemoBool && !recNoMemoBool);
}

// read strings from file; return true if and only if file read successfully
bool readStrings() {
	// open file for read given by filename
	FILE * file;
	file = fopen(filename, "r");
	// firstly we will measure the lengths of x and y before we read them in to memory
	if (file) { // file opened successfully
		// first measure length of x
		bool done = false;
		int i;
		do { // read from file until newline encountered
			i = fgetc(file); // get next character
			if (i==EOF) { // EOF encountered too early (this is first string) 
				// print error message, close file and return false
				printf("Incorrect file syntax\n");
				fclose(file);
				return false; 
			}
			if ((char) i=='\n' || (char) i=='\r') // newline encountered
				done = true; // terminate loop
			else // one more character
				xLen++; // increment length of x
		} while (!done);
		// next measure length of y
		if ((char) i=='\r')
			fgetc(file); // get rid of newline character
		done = false;
		do { // read from file until newline or EOF encountered
			int i = fgetc(file); // get next character
			if (i==EOF || (char) i=='\n' || (char) i=='\r') // EOF or newline encountered
				done = true; // terminate loop
			else // one more character
				yLen++; // increment length of y
		} while (!done);
		fclose(file);
		// if either x or y is empty then print error message and return false
		if (xLen==0 || yLen==0) {
			printf("Incorrect file syntax\n");
			return false;
		}
		// now open file again for read
		file = fopen(filename, "r");
		// allocate memory for x and y
		x = malloc(xLen * sizeof(char));
		y = malloc(yLen * sizeof(char));
		// read in x character-by-character
		for (i=0; i<xLen; i++)
			x[i]=fgetc(file);
		i = fgetc(file); // read in newline between strings and discard
		if ((char) i=='\r')
			fgetc(file); // read \n character and discard if previous character was \r
		// read in y character-by-character
		for (i=0; i<yLen; i++)
			y[i]=fgetc(file);
		// close file and return boolean indicating success
		fclose(file);
		return true;
	}
	else { // notify user of I/O error and return false
		printf("Problem opening file %s\n",filename);
		return false;
	}
}

// generate two strings x and y (of lengths xLen and yLen respectively) uniformly at random over an alphabet of size alphabetSize
void generateStrings() {
	// allocate memory for x and y
	x = malloc(xLen * sizeof(char));
	y = malloc(yLen * sizeof(char));
	// instantiate the pseudo-random number generator (seeded based on current time)
	srand(time(NULL));
	int i;
	// generate x, of length xLen
	for (i = 0; i < xLen; i++)
		x[i] = rand()%alphabetSize +'A';
	// generate y, of length yLen
	for (i = 0; i < yLen; i++)
		y[i] = rand()%alphabetSize +'A';
}

// free memory occupied by strings
void freeMemory()
{
	free(x);
	free(y);
}
//___________________________________________________________________________________________________________________________________________________________________
//Returns max of 2 numbers
int my_max(int a, int b)
{
  if(a>=b)
  {
    return a;
  }
  else
  {
    return b;
  }
}
//Return min of 2 numbers.
int my_min(int a, int b)
{
  if(a<=b)
  {
    return a;
  }
  else
  {
    return b;
  }
}

//X,Y, xLen, yLen are global variables so not in arguement. Will carry out LCS algorithm, iterative method based.
void it_lcs(int* arr[])
{ 
  int i,j,temp;
  //everything 0
  for (j=0; j<yLen; j++)
  {
    for (i=0; i<xLen; i++)
    {
      arr[j][i]=0;
    }
  }
  //computing the values
  for (j=1; j<=yLen; j++)
    {
      for (i=1; i<=xLen; i++)
        {
          if (x[i-1]==y[j-1])
            {
              arr[j][i]=arr[j-1][i-1]+1;
            }
          else
            {
              arr[j][i]=my_max(arr[j-1][i],arr[j][i-1]);
            }
        }
    }
}
// number of digits in number+1. -1 case it for memoisation case.
int length_num(int i)
{
  double x, res;
  int temp;
  if(i==-1)
  {
    return 2;
  }
  else
  {
    x=i;
    res=log10(x);
    temp=res+1;
    return temp;
  }
}

// Returns the number of digits+1 of the largest number in the array.
int max_log(int* arr[])
{
  int ans=0,i,j,temp;
  ans=length_num(my_max(xLen,yLen));
  for (j=0;j<yLen;j++)
  {
    for(i=0;i<xLen;i++)
    {
      ans=my_max(ans,length_num(arr[j][i]));
    }
  }
  return ans;
}

// prints a character, and gives sufficient spaces so that columns align up
void print_char(int spaces, char c)
{
  int temp;
  printf("%c", c);
  for(temp=0;temp<spaces;temp++)
    {
      printf(" ");
    }
}
// prints a integer, and gives sufficients spaces so that columns align up. For -1 we will print '-' with sufficient spaces.
void print_int(int spaces, int i)
{
  int temp, leftover;
  if(i>0)
  {
    printf("%d",i);
    leftover=spaces-length_num(i);
    for (temp=0;temp<=leftover;temp++)
    {
      printf(" ");
    }
  }
  else if(i==0)
  {
    printf("%d",0);
    for(temp=0;temp<spaces;temp++)
    {
      printf(" ");
    }
  }
  else if(i== -1)
  {
    printf("-");
    for(temp=0;temp<spaces;temp++)
    {
      printf(" ");
    }
  }
}
// This function will print the array, 'space' tell how many spaces, so that columns align
void print_array(int* arr[], int space)
{
  int i,j;
  print_char(space,' ');
  print_char(space,' ');
  print_char(space,' ');
  for (i=0;i<=xLen;i++)
  {
    print_int(space,i%10);
  }
  printf("\n");
  print_char(space,' ');
  print_char(space,' ');
  print_char(space,' ');
  print_char(space,' ');
  for(i=0;i<xLen;i++)
  {
    print_char(space,x[i]);
  }
  printf("\n");
  print_char(space,' ');
  print_char(space,' ');
  for(i=0;i<=xLen+1;i++)
  {
    for (j=0;j<=space;j++)
    {
      printf("_");
    }
  }
  printf("\n");
  print_int(space,0);
  print_char(space,' ');
  print_char(space,'|');
  for (i=0;i<=xLen;i++)
  {
    print_int(space,arr[0][i]); 
  }
  printf("\n");
  for(j=1;j<=yLen;j++)
  {
    print_int(space,j);
    print_char(space,y[j-1]);
    print_char(space,'|');
    for(i=0;i<=xLen;i++)
    {
      print_int(space,arr[j][i]);
    }
    printf("\n");
  }
}
// In case of LCS, this will print out an opitmal alignment(There are many alignments!).
void print_alignment(int* arr[],int l)
{
  int for_x[l], for_y[l];
  int i=xLen, j=yLen, len=l,temp;
  //Finding the LCS
  while(j>0 && i>0)
  {
    if(x[i-1]==y[j-1])
    {
      for_x[len-1]=i-1;
      for_y[len-1]=j-1;
      i--;
      j--;
      len--;
    }
    else
    {
      if(arr[j-1][i]==arr[j][i])
      {
        j--;
      }
      else
      {
        i--;
      }
    }
  }
  int to_print;
  //Will work our way step by step, will print x then '|' then y.
  //Line 1 print x with - at appropriate places.
  for(i=0;i<l;i++)
  {
    if(i==0)
    {
      to_print=for_x[i]+for_y[i]+1;
      for(j=0;j<for_x[i];j++)
      {
        printf("%c",x[j]);
      }
      for(j=for_x[i];j<to_print-1;j++)
      {
        printf("-");
      }
      printf("%c",x[for_x[i]]);
    }
    else
    {
      to_print=for_x[i]+for_y[i]-for_x[i-1]-for_y[i-1]-1;
      for(j=for_x[i-1]+1;j<for_x[i];j++)
      {
        printf("%c",x[j]);
      }
      for(j=for_x[i]-for_x[i-1]-1;j<to_print-1;j++)
      {
        printf("-");
      }
      printf("%c",x[for_x[i]]);
    }
  }
  to_print=xLen+yLen-for_x[l-1]-for_y[l-1]-2;
  for(j=for_x[l-1]+1;j<xLen;j++)
  {
    printf("%c",x[j]);
  }
  for(j=for_y[l-1]+1;j<yLen;j++)
  {
    printf("-");
  }
  printf("\n");
  //Line 2 print " " and "|"
  for(i=0;i<l;i++)
  {
    if(i==0)
    {
      to_print=for_x[i]+for_y[i]+1;
      for(j=0;j<to_print-1;j++)
      {
        printf(" ");
      }
      printf("|");
    }
    else
    {
      to_print=for_x[i]+for_y[i]-for_x[i-1]-for_y[i-1]-1;
      for(j=0;j<to_print-1;j++)
      {
        printf(" ");
      }
      printf("|");
    }
  }
  printf("\n");
  //Line 3 print y with '-' at appropriate places
  for(i=0;i<l;i++)
  {
    if(i==0)
    {
      to_print=for_x[i]+for_y[i]+1;
      for(j=for_y[i];j<to_print-1;j++)
      {
        printf("-");
      }
      for(j=0;j<for_y[i];j++)
      {
        printf("%c",y[j]);
      }
      printf("%c",y[for_y[i]]);
    }
    else
    {
      to_print=for_x[i]+for_y[i]-for_x[i-1]-for_y[i-1]-1;
      for(j=for_y[i]-for_y[i-1]-1;j<to_print-1;j++)
      {
        printf("-");
      }
      for(j=for_y[i-1]+1;j<for_y[i];j++)
        {
          printf("%c",y[j]);
        }
      printf("%c",y[for_y[i]]);
    }
  }
  to_print=xLen+yLen-for_x[l-1]-for_y[l-1]-1;
  for(j=for_x[l-1]+1;j<xLen;j++)
    {
      printf("-");
    }
  for(j=for_y[i-1]+1;j<yLen;j++)
    {
      printf("%c",y[j]);
    }
  printf("\n");
}

//x,y,xLen,yLen are global variables, so not arguements. this will carry out Smith Watermann algorithm, iterative method.
void it_sw(int *arr[])
{
  int i,j,temp;
  //everything 0
  for (j=0; j<=yLen; j++)
    {
      for (i=0; i<=xLen; i++)
        {
          arr[j][i]=0;
        }
    }
 //computing the/*  values 
   for (j=1; j<=yLen; j++) 
     { 
       for (i=1; i<=xLen; i++) 
         { 
           if (x[i-1]==y[j-1]) 
             { 
               temp=arr[j-1][i-1];
               arr[j][i]=temp+1; 
             } 
           else 
             { 
               temp=my_max(arr[j-1][i]-1,arr[j][i-1]-1);
               temp=my_max(arr[j-1][i-1],temp);
               temp=my_max(0,temp);
               arr[j][i]=temp;
             }
         }
     }
 }
//Find the maximum elelemt of the array, used in SW algorithm.
int max_of_array(int* arr[])
{
  int i,j,ans=0;
  for(j=0;j<=yLen;j++)
  {
    for(i=0;i<=xLen;i++)
    {
      ans=my_max(ans,arr[j][i]);
    }
  }
  return ans;
}
//x,y,xLen,yLen are global variables, so not arguements. this will carry out Edit Distance algorithm, iterative method
void it_ed(int *arr[])
{
  int i,j,temp;
  //everything 0
  for (j=0; j<=yLen; j++)
    {
      arr[j][0]=j;
    }
  for(i=0;i<=xLen;i++)
    {
      arr[0][i]=i;
    }
  //computing the/*  values 
  for (j=1; j<=yLen; j++) 
  { 
    for (i=1; i<=xLen; i++) 
    { 
      if (x[i-1]==y[j-1]) 
      { 
        arr[j][i]=arr[j-1][i-1];
      } 
      else 
      { 
        temp=my_min(arr[j-1][i],arr[j][i-1]);
        temp=my_min(arr[j-1][i-1],temp);
        arr[j][i]=temp+1;
      }
    }
  }
}
//For recursive without memoisation algorithms, how will go about them is we will use an array to store all the values, that array is initalised to 0 everywhere.
//Will call a void function so that entries are modified.


//This is implementation of Recursion without memoisation of LCS algorithm.
void r_lcs(int* arr[], int j, int i)
{
  arr[j][i]=arr[j][i]+1;
  if(i!=0 && j!=0)
  {
    if (x[i-1]==y[j-1])
    {
      r_lcs(arr,j-1,i-1);
    }
    else
    {
      r_lcs(arr,j,i-1);
      r_lcs(arr,j-1,i);
    }
  }
}

//This is implementation of Recursion without memoisation of ED algorithm.
void r_ed(int* arr[], int j, int i)
{
  arr[j][i]++;
  if(i!=0 && j!=0)
  {
    if (x[i-1] == y[j-1])
    {
      r_ed(arr, j-1, i-1);
    }
    else
    {
      r_ed(arr, j, i-1);
      r_ed(arr, j-1, i);
      r_ed(arr, j-1, i-1); 
    }
  }
}

//Finds the sum of all the entries in the matrix, which is used in recursion without memoisation case.
int sum_of_entries(int *arr[])
{
  int sum=0,j,i;
  for(j=0;j<=yLen;j++)
  {
    for(i=0;i<xLen;i++)
    {
      sum=sum+arr[j][i];
    }
  }
  return sum;
}

//For recursive with memoisation algorithms, how will go about them is we will use an array to store all the values, that array is initalised to 0 everywhere.
//Will call a void function so that entries are modified.

//Implementation of Recursion with memoisation version of LCS algorithm.
//This function will end with making value of m_v[j][i] genuine and the related necessary changes.
void m_lcs(int* m_v[], int* m_p[], int* m_b, int j, int i)
{
  if(is_valid(m_v,m_p,m_b,j,i,count)==0)
  {
    if(i==0 || j==0)
    {
      m_v[j][i]=0;
      m_p[j][i]=count;
      m_b[count]=i+(j*(xLen+1));
      count++;
    }
    else
    {
      if(x[i-1]==y[j-1])
      {
        if(is_valid(m_v,m_p,m_b,j-1,i-1)==0)
        {
          m_lcs(m_v,m_p,m_b,j-1,i-1);
        }
        m_v[j][i]=m_v[j-1][i-1]+1;
        m_p[j][i]=count;
        m_b[count]=i+(j*(xLen+1));
        count++;
      }
      else
      {
        if(is_valid(m_v,m_p,m_b,j,i-1)==0)
        {
          m_lcs(m_v,m_p,m_b,j,i-1);
        }
        if(is_valid(m_v,m_p,m_b,j-1,i)==0)
        {
          m_lcs(m_v,m_p,m_b,j-1,i);
        }
        m_v[j][i]=my_max(m_v[j-1][i],m_v[j][i-1]);
        m_p[j][i]=count;
        m_b[count]=i+(j*(xLen+1));
        count++;
      }
    }
  }
}

//This checks if given value at location [j][i] of m_v is genuine.
int is_valid(int* m_v[], int* m_p[], int* m_b, int j, int i)
{
  int k=m_p[j][i];
  int temp=m_b[k];
  if(k<0||k>count)
  {
    return 0;
  }
  else
  {
    if(temp== i+(j*(xLen+1)))
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
}

//Implementation of Recursion with memoisation version of ED algorithm.
//This function will end with making value of m_v[j][i] genuine and the related necessary changes.
void m_ed(int *m_v[], int *m_p[], int* m_b, int j, int i)
{
  if(is_valid(m_v,m_p,m_b,j,i)==0)
    {
      if(i==0 && j!=0)
        {
          m_v[j][i]=j;
          m_p[j][i]=count;
          m_b[count]=i+(j*(xLen+1));
          count++;
        }
      else if (j==0)
        {
          m_v[j][i]=i;
          m_p[j][i]=count;
          m_b[count]=i+(j*(xLen+1));
          count++;
        }
      else if(i!=0 && j!=0)
        {
          if(x[i-1]==y[j-1])
            {
              if(is_valid(m_v,m_p,m_b,j-1,i-1)==0)
                {
                  m_ed(m_v,m_p,m_b,j-1,i-1);
                }
              m_v[j][i]=m_v[j-1][i-1];
              m_p[j][i]=count;
              m_b[count]=i+(j*(xLen+1));
              count++;
            }
          else
            {
              if(is_valid(m_v,m_p,m_b,j,i-1)==0)
                {
                  m_ed(m_v,m_p,m_b,j,i-1);
                }
              if(is_valid(m_v,m_p,m_b,j-1,i)==0)
                {
                  m_ed(m_v,m_p,m_b,j-1,i);
                }
              if(is_valid(m_v,m_p,m_b,j-1,i-1)==0)
                {
                  m_ed(m_v,m_p,m_b,j-1,i-1);
                }
              m_v[j][i]=my_min(m_v[j-1][i],m_v[j][i-1]);
              m_v[j][i]=my_min(m_v[j-1][i-1],m_v[j][i])+1;
              m_p[j][i]=count;
              m_b[count]=i+(j*(xLen+1));
              count++;
            }
        }
    }

}

//_____________________________________________________________________________________________________________________________________________________________
// main method, entry point
int main(int argc, char *argv[])
{
	bool isIllegal = getArgs(argc, argv); // parse arguments from command line
  if (isIllegal) // print error and quit if illegal arguments
		printf("Illegal arguments\n");
	else
  {
    // int *it[yLen+1],*r[yLen+1],*m_v[yLen+1],*m_p[yLen+1];
    int i,j;
    int **it = (int **)malloc((yLen+1) * sizeof(int *));
    int **r = (int **)malloc((yLen+1) * sizeof(int *));
    int **m_v = (int **)malloc((yLen+1) * sizeof(int *));
    int **m_p = (int **)malloc((yLen+1) * sizeof(int *));
    for (i=0; i<yLen+1; i++)
    {
      it[i] = (int *)malloc((xLen+1) * sizeof(int));
      r[i] = (int *)malloc((xLen+1) * sizeof(int));
      m_v[i] = (int *)malloc((xLen+1) * sizeof(int));
      m_p[i] = (int *)malloc((xLen+1) * sizeof(int));
    }
    int *m_b= (int*)malloc((xLen+1)*(yLen+1)*sizeof(int));
    int space_it,space_r,space_m;
    int temp;
    long long int total;
    double time=0,pro_1=0,pro_2=0;
    /*
    for (j=0; j<yLen+1; j++)
    {
      r[j] = (int *)malloc((xLen+1) * sizeof(int));
      m_v[j] = (int *)malloc((xLen+1) * sizeof(int));
      m_p[j] = (int *)malloc((xLen+1) * sizeof(int));
      it[j] = (int *)malloc((xLen+1) * sizeof(int));
    }
    */
		printf("%s\n", alg_desc); // confirm algorithm to be executed
		bool success = true;
    if (genStringsBool)
			generateStrings(); // generate two random strings
		else
			success = readStrings(); // else read strings from file
		if (success)
    {
      // do not proceed if file input was problematic
			// confirm dynamic programming type
			// these print commamds are just placeholders for now
      // check if algorithm to execute if LCS
      if(alg_type==LCS)
      {
        //do we have to print DP tables??
        if(printBool)
        {
          //Method to execute the algorithm in is iterative
          if(iterBool)
          {
            printf("Iterative version\n");;
            clock_t start=clock();
            it_lcs(it);
            space_it=max_log(it);
            printf("Length of a longest common subsequence is: %d\n",it[yLen][xLen]);
            printf("Dynamic programming table:\n");
            print_array(it,space_it);
            printf("\nOptimal alignment:\n");
            print_alignment(it,it[yLen][xLen]);
            clock_t stop =clock();
            time = (stop-start)/CLOCKS_PER_SEC;
            printf("\nTime taken: %f seconds\n\n",time);
          }
          //Method to execute the algorithm in is Recursion without memoisation
          if(recNoMemoBool)
          {
            printf("Recursive version without memoisation\n");
            clock_t start=clock();
            for(j=0;j<=yLen;j++)
              {
                for(i=0;i<=xLen;i++)
                  {
                    r[j][i]=0;
                  }
              }
            r_lcs(r,yLen,xLen);
            space_r=max_log(r);
            total=sum_of_entries(r);
            print_array(r,space_r);
            printf("Total number of times a table entry computed: %lld\n", total);
            clock_t stop =clock();
            time = (stop-start)/CLOCKS_PER_SEC;
            printf("Time taken: %f seconds\n\n", time);
          }
          //Method to execute the algorithm in is recursion with memoisation
          if(recMemoBool)
          {
            count=0;
            printf("Recursive version with memoisation\n");
            clock_t start=clock();
            m_lcs(m_v,m_p,m_b,yLen,xLen);
            for(j=0;j<=yLen;j++)
            {
              for(i=0;i<=xLen;i++)
              {
                if(is_valid(m_v,m_p,m_b,j,i)==0)
                {
                  m_v[j][i]=-1;
                }
              }
            }
            space_m=max_log(m_v);
            printf("\nLength of longest common subsequence is: %d\n",m_v[yLen][xLen]);
            printf("Dynamic programming table:\n");
            print_array(m_v,space_m);
            print_alignment(m_v,m_v[yLen][xLen]);
            printf("Number of table entries computed: %lld\n", count);
            pro_1=(xLen+1)*(yLen+1);
            pro_2=count*100;
            printf("Proportion of table computed: %f\% \n", pro_2/pro_1);
            clock_t stop =clock();
            time = (stop-start)/CLOCKS_PER_SEC;
            printf("Time taken: %f seconds\n\n", time);
          }
        }
        //do we have to print DP tables??
        else
        {
          //Method to execute the algorithm in is Iterative
          if(iterBool)
          {
            printf("Iterative version\n");
            clock_t start=clock();
            it_lcs(it);
            printf("Length of a longest common subsequence is: %d\n",it[yLen][xLen]);
            clock_t stop =clock();
            time = (stop-start)/CLOCKS_PER_SEC;
            printf("\nTime taken: %f seconds\n\n",time);
          }
          //Method to execute the algorithm in is recursion without memoisation
          if(recNoMemoBool)
          {
            printf("Recursive version without memoisation\n");
            clock_t start=clock();
            for(j=0;j<=yLen;j++)
            {
              for(i=0;i<=xLen;i++)
              {
                r[j][i]=0;
              }
            }
            r_lcs(r,yLen,xLen);
            total=sum_of_entries(r);
            printf("Total number of times a table entry computed: %lld\n", total);
            clock_t stop =clock();
            time = (stop-start)/CLOCKS_PER_SEC;
            printf("\nTime taken: %f seconds\n\n",time);
          }
          //Method to execute the algorithm in is recursion with memoisation
          if(recMemoBool)
          {
            count=0;
            printf("Recursive version with memoisation\n");
            clock_t start=clock();
            m_lcs(m_v,m_p,m_b,yLen,xLen);
            clock_t stop =clock();
            printf("Length of longest common subsequence is: %d\n",m_v[yLen][xLen]);
            printf("Number of table entries computed: %lld\n", count);
            pro_1=(xLen+1)*(yLen+1);
            pro_2=count*100;
            printf("Proportion of table computed: %f\%\n",  pro_2/pro_1);
            time = (stop-start)/CLOCKS_PER_SEC;
            printf("Time taken: %f seconds\n\n", time);
          }
        }
      }
      //Check if the algorithm to execute is ED.
      else if(alg_type==ED)
      {
        //do we have to print DP tables??
        if(printBool)
        {
          //Method to execute the algorithm in is Iterative
          if(iterBool)
          {
            printf("Iterative version\n");
            clock_t start=clock();
            it_ed(it);
            space_it=max_log(it);
            printf("Edit distance  is: %d\n",it[yLen][xLen]);
            printf("Dynamic programming table:\n");
            print_array(it,space_it);
            clock_t stop =clock();
            time = (stop-start)/CLOCKS_PER_SEC;
            printf("\nTime taken: %f seconds\n\n",time);
          }
          //Method to execute the algorithm in is recursion without memoisation
          if(recNoMemoBool)
          {
            printf("Recursive version without memoisation\n");
            clock_t start=clock();
            r_ed(r,yLen,xLen);
            space_r=max_log(r);
            total=sum_of_entries(r);
            printf("Dynamic programming table:\n");
            print_array(r,space_r);
            printf("\nTotal number of times a table entry computed: %lld\n", total);
            clock_t stop =clock();
            time = (stop-start)/CLOCKS_PER_SEC;
            printf("\nTime taken: %f seconds\n\n", time);
          }
          //Method to execute the algorithm in is recursion with memoisation
          if(recMemoBool)
          {
            printf("Recursive version with memoisation\n");
            count=0;
            clock_t start=clock();
            m_ed(m_v,m_p,m_b,yLen,xLen);
            for(j=0;j<=yLen;j++)
            {
              for(i=0;i<=xLen;i++)
              {
                if(is_valid(m_v,m_p,m_b,j,i)==0)
                {
                  m_v[j][i]=-1;
                }
              }
            }
            space_m=max_log(m_v);
            printf("\nEdit distance is: %d\n",m_v[yLen][xLen]);
            printf("Dynamic programming table:\n");
            print_array(m_v,space_m);
            printf("Number of table entries computed: %lld\n", count);
            pro_1=(xLen+1)*(yLen+1);
            pro_2=count*100;
            printf("Proportion of table computed: %f\% \n", pro_2/pro_1);
            clock_t stop =clock();
            time = (stop-start)/CLOCKS_PER_SEC;
            printf("Time taken: %f seconds\n\n", time);
          }
        }
        //do we have to print DP tables??
        else
        {
          //Method to execute the algorithm in is Iterative
          if(iterBool)
          {
            printf("Iterative version\n");
            clock_t start=clock();
            it_ed(it);
            printf("Edit distance  is: %d\n",it[yLen][xLen]);
            clock_t stop =clock();
            time = (stop-start)*1000.0/CLOCKS_PER_SEC;
            printf("\n Time taken: %f\n\n",time);
          }
          //Method to execute the algorithm in is recursion without memoisation
          if(recNoMemoBool)
          {
            printf("Recursive version without memoisation\n");
            clock_t start=clock();
            r_ed(r,yLen,xLen);
            total=sum_of_entries(r);
            printf("\nTotal number of times a table entry computed: %lld", total);
            clock_t stop =clock();
            time = (stop-start)/CLOCKS_PER_SEC;
            printf("\nTime taken: %f seconds\n\n", time);
          }
          //Method to execute the algorithm in is recursion without m
          if(recMemoBool)
          {
            printf("Recursive version with memoisation\n");
            clock_t start=clock();
            m_ed(m_v,m_p,m_b,yLen,xLen);
            clock_t stop =clock();
            space_m=max_log(m_v);
            printf("\nEdit distance is: %d\n",m_v[yLen][xLen]);
            printf("Number of table entries computed: %lld\n", count);
            pro_1=(xLen+1)*(yLen+1);
            pro_2=count*100;
            printf("Proportion of table computed: %f\% \n", pro_2/pro_1);
            time = (stop-start)/CLOCKS_PER_SEC;
            printf("\nTime taken: %f seconds\n\n",time);
          }
        }
      }
      //Last choice is that algorithm to execute if SW algorithm
      else
      {
        //do we have to print DP tables??
        if(printBool)
        {
          if(iterBool)
          {
            printf("Iterative version\n");
            clock_t start=clock();
            it_sw(it);
            space_it=max_log(it);
            printf("Length of highest scoring local similarity is: %d\n",max_of_array(it));
            printf("Dynamic programming table:\n");
            print_array(it,space_it);
            clock_t stop =clock();
            time = (stop-start)/CLOCKS_PER_SEC;
            printf("\nTime taken: %f seconds\n\n",time);
          }
        }
        //do we have to print DP tables??
        else
        {
          if(iterBool)
          {
            printf("Iterative version\n");
            clock_t start=clock();
            it_sw(it);
            printf("Length of highest scoring local similarity is: %d\n",max_of_array(it));
            clock_t stop =clock();
            time = (stop-start)/CLOCKS_PER_SEC;
            printf("\nTime taken: %f seconds\n\n",time);
          }
        }
      }
      
			freeMemory(); // free memory occupied by strings
		}
	}
	return 0;
}
