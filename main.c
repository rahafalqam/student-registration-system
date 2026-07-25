//Rahaf Alqam 1232633 Sec. 2
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//Original code provided by the author Mark Allen Wiess.
//Modified by Dr. Radi Jarrar >> for AVL code


// linked list definition
typedef struct courseReg* CourseNode; // bc one student may has more than one course (choose linked bc dont know num. of course)
struct courseReg {
    char courseCode[20];     // COMP242
    char courseTitle[80];    // Operating Systems
    int  creditHours;        // 3
    char semester[20];       //  Fall 2024
    CourseNode next;         // next course
};
typedef CourseNode CourseList;

// Course Linked List function
CourseList makeCourseList();
CourseNode createCourse(char courseCode[], char courseTitle[], int creditHours, char semester[]);
int isEmptyCourseList(CourseList L);
void insertCourseLast(CourseList L, CourseNode C);
void deleteCourse(CourseList L, char courseCode[]);
void printCourses(CourseList L);
int courseCount(CourseList L);
int HasCourse(char courseCode[], CourseList L);
void FreeCourseList(CourseList L);


// AVL definition
typedef struct AVLnode *AVLNode;
struct AVLnode {
    int id;   // key
    char name[60];
    char major[40];
    CourseList regs;  // linked list of courses
    AVLNode Left;
    AVLNode Right;
    int Height; //Balance information
};


// AVL function
AVLNode MakeEmpty( AVLNode T );
AVLNode Find( int id, AVLNode T );
AVLNode FindMin( AVLNode T );
AVLNode FindMax( AVLNode T );
int Height( AVLNode P );
int Max( int Lhs, int Rhs );

AVLNode SingleRotateWithLeft( AVLNode K2 );
AVLNode SingleRotateWithRight( AVLNode K1 );
AVLNode DoubleRotateWithLeft( AVLNode K3 );
AVLNode DoubleRotateWithRight( AVLNode K1 );

AVLNode InsertReg( char name[], int id, char major[],
                   char courseCode[], char courseTitle[],
                   int creditHours, char semester[],
                   AVLNode T );

void PrintInOrder( AVLNode t );


// Menu
void AVLMenu();

// Option 1
AVLNode LoadFile(AVLNode root);

// Option 2
AVLNode InsertNewRegistration(AVLNode root);

// Option 3
AVLNode FindByName(char name[], AVLNode T);
CourseNode FindCourse(CourseList L, char courseCode[]);
void UpdateHelper(AVLNode T, char name[], char newName[], char newMajor[], int *found);
void FindStudentAndUpdate(AVLNode root);

// Option 4
void inorderStudentsInCourse(AVLNode T, char courseCode[], int *found);
void ListAllStudentsInSameCourse(AVLNode root);

// Option 5
void DeleteStudentRegistration(AVLNode root);

// Option 6
void SaveStudentsToHashFile(AVLNode root);
void SaveToFileHelper(AVLNode T, FILE *out);


//Hash
#define NAME_LEN  60
#define MAJOR_LEN 40

typedef struct {
    char name[NAME_LEN]; //key
    int  id;
    char major[MAJOR_LEN];
    char status;   // 0 = empty , 1 = occupied , -1 = deleted
} Cell;


/* Struct of the Hash Table */       // from book (3rd edition)
typedef struct Hash {
    Cell *hash_table;
    int loaded_Data;
    int table_size;
} Hash;


//hash function
void InitHash(Hash *H, int size);
int HashFunction(char name[], int table_size);

void InsertStudentDirectToHash(Hash *H, char name[], int id, char major[]);
void LoadHashFromFile(Hash *H);

//  option 1
void PrintHashTable(Hash *H);
void PrintStudentCoursesFromFile(int id);

// option 2
void PrintHashSize(Hash *H);

// option 3
void PrintHashFunction();

//  option 4
void InsertStudentToHash(Hash *H, AVLNode root);

//  option 5
int  SearchHash(Hash *H, char name[]);
void SearchStudentInHash(Hash *H, AVLNode root);

//  option 6
void DeleteFromHash(Hash *H, char name[]);

// option 7
void SaveAVLToRegFileHelper(AVLNode T, FILE *out);

// hash menu
void HashMenuLoop(Hash *H, AVLNode root);


int main()
{
    AVLNode root = NULL;
    Hash H;
    InitHash(&H, 101);

    char choice[10];

    while (1)
    {
        AVLMenu();
        printf("\nPLEASE, Enter your choice\n");
        scanf("%s", choice);

        switch (atoi(choice))
        {
            case 1:
                root = LoadFile(root);
                break;

            case 2:
                root = InsertNewRegistration(root);
                break;

            case 3:
                FindStudentAndUpdate(root);
                break;

            case 4:
                ListAllStudentsInSameCourse(root);
                break;

            case 5:
                DeleteStudentRegistration(root);
                break;

            case 6:
                if (root == NULL) {
                    printf("Tree is empty. Load file first.\n");
                    break;
                }
                SaveStudentsToHashFile(root);
                break;

            case 7:
                if (root == NULL) {
                    printf("Load students first.\n");
                    break;
                }

                InitHash(&H, 101);
                LoadHashFromFile(&H);
                HashMenuLoop(&H, root);
                break;

            case 0:
                printf("Exit !!\n");
                return 0;

            default:
                printf("Invalid choice!\n");
        }
    }
}


//main method
void AVLMenu() {
    printf("\n==== AVL Menu ====\n");
    printf("1.Read and Load reg.txt file\n");
    printf("2. Insert new registration.\n");
    printf("3. Find student (using name) and update the information\n");
    printf("4. List all student in same class\n");
    printf("5. Delete a student registration\n");
    printf("6. Save all in file(students_hash.data)\n");
    printf("7.Go to Hash menu\n");
    printf("8.Exit\n");
}

// option 1 done
AVLNode LoadFile(AVLNode root){
    FILE* file = fopen("reg.txt", "r"); // open file
    if (file == NULL) { // check
        printf("Cannot open reg.txt file!\n");
        return root;
    }

    char line[512];

    while (fgets(line, sizeof(line), file))
    {
        // fields in order:
        // Name#ID#Major#CourseCode#CourseTitle#CreditHours#Semester

        char name[60], major[40], courseCode[20], courseTitle[80], semester[20];
        int id, creditHours;

        char *token;

        token = strtok(line, "#");
        if (token == NULL) continue;
        strcpy(name, token);

        token = strtok(NULL, "#");
        if (token == NULL) continue;
        id = atoi(token);

        token = strtok(NULL, "#");
        if (token == NULL) continue;
        strcpy(major, token);

        token = strtok(NULL, "#");
        if (token == NULL) continue;
        strcpy(courseCode, token);

        token = strtok(NULL, "#");
        if (token == NULL) continue;
        strcpy(courseTitle, token);

        token = strtok(NULL, "#");
        if (token == NULL) continue;
        creditHours = atoi(token);

        token = strtok(NULL, "#");
        if (token == NULL) continue;
        strcpy(semester, token);

        // remove \n from semester (last field)
        int x = strlen(semester);
        if (x > 0 && (semester[x - 1] == '\n' || semester[x - 1] == '\r')) {
            semester[x - 1] = '\0';
        }

        // insert into AVL (key = id)
        root = InsertReg(name, id, major, courseCode, courseTitle, creditHours, semester, root);
    }

    fclose(file);
    printf("reg.txt loaded successfully!\n");
    return root;
}


//option 2 work
AVLNode InsertNewRegistration(AVLNode root){

    int id;
    int creditHours;
    char name[60];
    char major[40];
    char courseCode[20];
    char courseTitle[80];
    char semester[20];

    AVLNode student = NULL;

    while (1) { // to still repeat until user choose valid input
        printf("\nEnter Student ID: ");
        if (scanf("%d", &id) == 1)
            break;

        printf("Invalid ID! Please enter numbers only.\n");
        while (getchar() != '\n');      }

    student = Find(id, root); // to check if we have this student

    if (student != NULL) {
        printf("Student already exists.\n");
        printf("Adding a new course for this student.\n");

        printf("Enter Course Code: ");
        scanf("%s", courseCode);

        if (HasCourse(courseCode, student->regs)) { // to check if student has this course befor
            printf("This student is already registered in this course.\n");
            return root;
        }

        getchar(); // to remove /n

        printf("Enter Course Title: ");
        gets(courseTitle);

        while (1) {
            printf("Enter Credit Hours: ");
            if (scanf("%d", &creditHours) == 1)
                break;

            printf("Invalid input! Please enter numbers only.\n");
            while (getchar() != '\n');
        }

        getchar();

        printf("Enter Semester: ");
        gets(semester);

        // make new course node and add it at last of linked list
        insertCourseLast(student->regs,createCourse(courseCode, courseTitle, creditHours, semester));

        printf("Course added successfully.\n");
        return root; // tree not change bc we already have this student
    }

    getchar();

    printf("Enter Student Name: ");
    gets(name);

    printf("Enter Major: ");
    gets(major);

    printf("Enter Course Code: ");
    scanf("%s", courseCode);

    getchar();

    printf("Enter Course Title: ");
    gets(courseTitle);

    while (1) {
        printf("Enter Credit Hours: ");
        if (scanf("%d", &creditHours) == 1)
            break;

        printf("Invalid input! Please enter numbers only.\n");
        while (getchar() != '\n');
    }

    getchar();

    printf("Enter Semester: ");
    gets(semester);

    root = InsertReg(name, id, major,courseCode, courseTitle,creditHours, semester,root);

    printf("New student registration added successfully.\n");
    return root;
}



//option 3 (helper)
AVLNode FindByName(char name[], AVLNode T){// find student by name and return his node
    AVLNode P;

    if (T == NULL)
        return NULL;

    P = FindByName(name, T->Left);
    if (P != NULL)
        return P;

    if (strcmp(T->name, name) == 0)
        return T;

    return FindByName(name, T->Right);
}

// 3 helper
CourseNode FindCourse(CourseList L, char courseCode[]){ // if we have student and we need to know his course
    if (L == NULL)
        return NULL;

    CourseNode c = L->next; // header
    while (c != NULL) {
        if (strcmp(c->courseCode, courseCode) == 0)
            return c;
        c = c->next;
    }
    return NULL;
}


//option 3 (helper)
void UpdateHelper(AVLNode T, char name[], char newName[], char newMajor[], int *found){//find student by name and update his n / m and stop after first one
                                    // found to provide update for more than one if we have more than one st. with this name
    if (T == NULL || *found == 1)
        return;

    UpdateHelper(T->Left, name, newName, newMajor, found);

    if (*found == 1) // we found so stop
        return;

    if (strcmp(T->name, name) == 0) { // to compar between this two if it similar ( user enter and what i have )

        strcpy(T->name, newName);
        strcpy(T->major, newMajor);

        *found = 1;
         return;
    }

    UpdateHelper(T->Right, name, newName, newMajor, found);
}


//option 3 //work
void FindStudentAndUpdate(AVLNode root){
    char name[60];
    char newName[60];
    char newMajor[40];
    char courseCode[20];
    char newTitle[80];
    char newSemester[20];
    int newCH;
    int choice;

    if (root == NULL) {
        printf("Tree is empty. Load file first.\n");
        return;
    }

    printf("\nEnter Student Name to search: ");
    getchar(); //remove /n
    gets(name);

    AVLNode student = FindByName(name, root); //to search

    if (student == NULL) {
        printf("Student not found.\n");
        return;
    }

    printf("\nStudent found:\n"); // print his information
    printf("ID   : %d\n", student->id);
    printf("Name : %s\n", student->name);
    printf("Major: %s\n", student->major);

    if (student->regs == NULL || student->regs->next == NULL)//not have
        printf("No courses registered.\n");
    else
        printCourses(student->regs); // print all courses

    do {
        printf("\nDo you want to update this student?\n");
        printf("1. Yes\n");
        printf("0. No\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
    } while (choice != 0 && choice != 1);

    if (choice == 0) {
        printf("No update done.\n");
        return;
    }

    getchar();

    printf("\nEnter new Student Name: ");
    gets(newName);
    strcpy(student->name, newName);

    printf("Enter new Major: ");
    gets(newMajor);
    strcpy(student->major, newMajor);

    CourseNode c = NULL;

    while (c == NULL) {
        printf("\nEnter Course Code to update: ");
        scanf("%s", courseCode);

        c = FindCourse(student->regs, courseCode);

        if (c == NULL)
            printf("Invalid course code! Please try again.\n");
    }

    getchar();

    printf("Enter new Course Title: ");
    gets(newTitle);

    while (1) {
        printf("Enter new Credit Hours: ");
        if (scanf("%d", &newCH) == 1)
            break;

        printf("Invalid number! Try again.\n");
        while (getchar() != '\n');
    }

    getchar();

    printf("Enter new Semester: ");
    gets(newSemester);

    strcpy(c->courseTitle, newTitle);
    c->creditHours = newCH;
    strcpy(c->semester, newSemester);

    printf("\nStudent and course updated successfully!\n");
}


//option 4 (helper)
void inorderStudentsInCourse(AVLNode T, char courseCode[], int *found){ // all student in order
    if (T == NULL)

        return;

    inorderStudentsInCourse(T->Left, courseCode, found); // check from left

    if (HasCourse(courseCode, T->regs)) {
        printf("\nStudent:\n");
        printf("ID : %d\n", T->id);
        printf("Name : %s\n", T->name);
        printf("Major: %s\n", T->major);
    }

    inorderStudentsInCourse(T->Right, courseCode, found);
}

//option 4 //work
void ListAllStudentsInSameCourse(AVLNode root){

    char courseCode[20];
    int found = 0;

    if (root == NULL) {
        printf("Tree is empty. Load file first.\n");
        return;
    }

    printf("Enter Course Code: ");
    scanf("%s", courseCode);

    inorderStudentsInCourse(root, courseCode, &found);// if found more than 0 >> found student
    found++;

    if (found == 0)
        printf("\nNo students found in this course.\n");
    else
        printf("\nStudents registered successfully.\n");

}

//option 5 //work
void DeleteStudentRegistration(AVLNode root){

    int id;
    char courseCode[20];

    if (root == NULL) {
        printf("There is no students added yet.\n");
        return;
    }

    printf("Enter Student ID: ");
    scanf("%d", &id);

    AVLNode s = Find(id, root);//to search
    if (s == NULL) {
        printf("Student not found.\n");
        return;
    }

    if (s->regs == NULL || s->regs->next == NULL) {
        printf("This student has no courses.\n");
        return;
    }

    printf("Enter Course Code to delete: ");
    scanf("%s", courseCode);

    if (!HasCourse(courseCode, s->regs)) {// what user enter =! what i have
        printf("This course is not registered for this student.\n");
        return;
    }

    deleteCourse(s->regs, courseCode);

    printf("Course deleted successfully.\n");
}


//option 6 (helper)
void SaveToFileHelper(AVLNode T, FILE *out){// save all information of reg
    if (T == NULL)
        return;

    SaveToFileHelper(T->Left, out);

    CourseNode c = T->regs->next;  // first course

    while (c != NULL) {

       // all student in order

       fprintf(out, "%s", T->name);
        fprintf(out, "#");

        fprintf(out, "%d", T->id);
        fprintf(out, "#");

        fprintf(out, "%s", T->major);
        fprintf(out, "#");

        fprintf(out, "%s", c->courseCode);
        fprintf(out, "#");

        fprintf(out, "%s", c->courseTitle);
        fprintf(out, "#");

        fprintf(out, "%d", c->creditHours);
        fprintf(out, "#");

        fprintf(out, "%s", c->semester);
        fprintf(out, "\n");

        c = c->next;
    }

    SaveToFileHelper(T->Right, out);
}


// option 6 work
void SaveStudentsToHashFile(AVLNode root){
    FILE *out = fopen("students_hash.data", "w");
    if (out == NULL) {
        printf("Cannot open students_hash.data file!\n");
        return;
    }

    SaveToFileHelper(root, out);

    fclose(out);
    printf("students_hash.data saved successfully with all courses.\n\n");
}


//+++++++++++++++++++=============+++++++++===

//method for linked list
CourseList makeCourseList() {
    CourseList L = (CourseList)malloc(sizeof(struct courseReg)); // space for header node
    if (!L) { // not have memory
        printf("Out of memory!\n");
        exit(1);
    }
    L->next = NULL; // pointer of header -> next
    return L;
}

//method
CourseNode createCourse(char courseCode[], char courseTitle[], int creditHours, char semester[]) {
    CourseNode c = (CourseNode)malloc(sizeof(struct courseReg));
    if (!c) {
        printf("Out of memory!\n");
        exit(1);
    }

    strcpy(c->courseCode, courseCode);
    strcpy(c->courseTitle, courseTitle);
    c->creditHours = creditHours;
    strcpy(c->semester, semester);

    c->next = NULL; // last
    return c;
}

//method
int isEmptyCourseList(CourseList L) {
    return (L == NULL || L->next == NULL); // return 1 (empty)
}

//method
void insertCourseLast(CourseList L, CourseNode C) {
    if (L == NULL) { // check list exists
        printf("Error: Course list is NULL.\n");
        return;
    }
    if (C == NULL) { // check course node
        printf("Error: Course is NULL\n");
        return;
    }

    CourseNode current = L; // header
    while (current->next != NULL) {
        current = current->next; // move to next node
    }

    current->next = C;
    C->next = NULL; // last
}

//method
void deleteCourse(CourseList L, char courseCode[]) {
    if (L == NULL) {
        printf("Course list not created!\n");
        return;
    }

    CourseNode prev = L;       // header
    CourseNode curr = L->next; // first course

    // search by course code
    while (curr != NULL && strcmp(curr->courseCode, courseCode) != 0) {
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) { // not found course
        printf("Course not found!\n");
        return;
    }

    prev->next = curr->next;
    free(curr);
}

//method
void printCourses(CourseList L) {
    if (L == NULL || L->next == NULL) {
        printf("No courses.\n");
        return;
    }

    CourseNode c = L->next; // start with first course not header

    printf("Courses:\n");

    while (c != NULL) {
        printf("Code: %s | Title: %s | CH: %d | Semester: %s\n",
               c->courseCode, c->courseTitle, c->creditHours, c->semester);
        c = c->next;
    }
}

//method
int courseCount(CourseList L) {
    int count = 0;
    if (L == NULL) return 0;

    CourseNode c = L->next;
    while (c != NULL) {
        count++;
        c = c->next;
    }
    return count;
}

//method to show all student in one course
int HasCourse(char courseCode[], CourseList L){
    CourseNode P;

    if (L == NULL)
        return 0;

    P = L->next;

    while (P != NULL)
    {
        if (strcmp(P->courseCode, courseCode) == 0)
            return 1;

        P = P->next;
    }

    return 0;
}

//method
void FreeCourseList(CourseList L)
{
    CourseNode P, Temp;

    if (L == NULL)
        return;

    P = L->next;

    while (P != NULL)
    {
        Temp = P;
        P = P->next;
        free(Temp);
    }

    free(L); /* free header */
}

//=+++++++++++++++++++++++++++++++++++++++++++

// AVL function
AVLNode MakeEmpty( AVLNode T )
{
    if( T != NULL )
    {
        MakeEmpty( T->Left );
        MakeEmpty( T->Right );

        //Free the linked list of courses first
        FreeCourseList( T->regs );

        free( T );
    }
    return NULL;
}


AVLNode Find( int id, AVLNode T )
{
    if( T == NULL )
        return NULL;

    if( id < T->id )
        return Find( id, T->Left );
    else if( id > T->id )
        return Find( id, T->Right );
    else
        return T;
}


AVLNode FindMin( AVLNode T )
{
    if( T == NULL )
        return NULL;
    else if( T->Left == NULL )
        return T;
    else
        return FindMin( T->Left );
}

AVLNode FindMax( AVLNode T )
{
    if( T != NULL )
        while( T->Right != NULL )
            T = T->Right;

    return T;
}

/* START: fig4_36.txt */
int Height( AVLNode P )
{
    if( P == NULL )
        return -1;
    else
        return P->Height;
}

int Max( int Lhs, int Rhs )
{
    return Lhs > Rhs ? Lhs : Rhs;
}


/* START: fig4_39.txt */
/* This function can be called only if K2 has a left child */
/* Perform a rotate between a node (K2) and its left child */
/* Update heights, then return new root */

AVLNode SingleRotateWithLeft( AVLNode K2 )
{
AVLNode K1;

K1 = K2->Left;
K2->Left = K1->Right;
K1->Right = K2;

K2->Height = Max( Height( K2->Left ), Height( K2->Right ) ) + 1;
K1->Height = Max( Height( K1->Left ), K2->Height ) + 1;

return K1;
}


/* This function can be called only if K1 has a right child */
/* Perform a rotate between a node (K1) and its right child */
/* Update heights, then return new root */

AVLNode SingleRotateWithRight( AVLNode K1 )
{
AVLNode K2;

K2 = K1->Right;
K1->Right = K2->Left;
K2->Left = K1;

K1->Height = Max( Height( K1->Left ), Height( K1->Right ) ) + 1;
K2->Height = Max( Height( K2->Right ), K1->Height ) + 1;

return K2;
}

/* START: fig4_41.txt */
/* This function can be called only if K3 has a left */
/* child and K3's left child has a right child */
/* Do the left-right double rotation */
/* Update heights, then return new root */

AVLNode DoubleRotateWithLeft( AVLNode K3 )
{
/* Rotate between K1 and K2 */
K3->Left = SingleRotateWithRight( K3->Left );

/* Rotate between K3 and K2 */
return SingleRotateWithLeft( K3 );
}
/* END */

/* This function can be called only if K1 has a right */
/* child and K1's right child has a left child */
/* Do the right-left double rotation */
/* Update heights, then return new root */

AVLNode DoubleRotateWithRight( AVLNode K1 )
{
/* Rotate between K3 and K2 */
K1->Right = SingleRotateWithLeft( K1->Right );

/* Rotate between K1 and K2 */
return SingleRotateWithRight( K1 );
}

/* START: fig4_37.txt */
AVLNode InsertReg( char name[], int id, char major[],
                   char courseCode[], char courseTitle[],
                   int creditHours, char semester[],
                   AVLNode T ){
    if( T == NULL ){
        // Create and return a one-node tree
        T = (AVLNode)malloc( sizeof( struct AVLnode ) );
        if( T == NULL )
            printf( "Out of space!!!" );
        else{
            T->id = id;
            strcpy( T->name, name );
            strcpy( T->major, major );

            T->regs = makeCourseList();
            insertCourseLast( T->regs,
                              createCourse( courseCode, courseTitle, creditHours, semester ) );

            T->Height = 0;
            T->Left = T->Right = NULL;
        }
    }
    else if( id < T->id )
    {
        T->Left = InsertReg( name, id, major, courseCode, courseTitle, creditHours, semester, T->Left );
        if( Height( T->Left ) - Height( T->Right ) == 2 )
            if( id < T->Left->id )
                T = SingleRotateWithLeft( T );
            else
                T = DoubleRotateWithLeft( T );
    }
    else if( id > T->id )
    {
        T->Right = InsertReg( name, id, major, courseCode, courseTitle, creditHours, semester, T->Right );
        if( Height( T->Right ) - Height( T->Left ) == 2 )
            if( id > T->Right->id )
                T = SingleRotateWithRight( T );
            else
                T = DoubleRotateWithRight( T );
    }
    else
    {
        // Else id is in the tree already; we'll add the course
        insertCourseLast( T->regs,
                          createCourse( courseCode, courseTitle, creditHours, semester ) );
    }

    T->Height = Max( Height( T->Left ), Height( T->Right ) ) + 1;
    return T;
}
/* END */

//in-order print to see the elements
void PrintInOrder( AVLNode t )
{
    if( t != NULL )
    {
        PrintInOrder( t->Left );

        printf("ID: %d\tName: %s\tMajor: %s\n", t->id, t->name, t->major);
        printCourses( t->regs );   // print registered courses for this student

        PrintInOrder( t->Right );
    }
}


//*************&^&$^&$$%^&^%%&^(*&&((*)^&#%***

// HASH fun.
void InitHash(Hash *H, int size){//to initialize table
    int i;

    H->table_size = size;
    H->loaded_Data = 0;

    H->hash_table = (Cell *)malloc(size * sizeof(Cell));
    if (H->hash_table == NULL) {
        printf("Out of memory!\n");
        return;
    }

    for (i = 0; i < size; i++) {
        H->hash_table[i].status = 0; // EMPTY
    }
}

//helper
int HashFunction(char name[], int table_size)
{
    int sum = 0;
    for (int i = 0; name[i] != '\0'; i++)
        sum += name[i];

    return sum % table_size;
}

// helper
void InsertStudentDirectToHash(Hash *H, char name[], int id, char major[]){// add student to hash table

    int index = HashFunction(name, H->table_size);

    while (H->hash_table[index].status == 1) { //collison

        if (strcmp(H->hash_table[index].name, name) == 0 && H->hash_table[index].id == id) { //check if it same st. or same name only
            return;   //same student
        }

        index = (index + 1) % H->table_size;    // Linear Probing

    }

    strcpy(H->hash_table[index].name, name);
    H->hash_table[index].id = id;
    strcpy(H->hash_table[index].major, major);
    H->hash_table[index].status = 1; // become in hash

    H->loaded_Data++;
}

//forrrrr hash file
void LoadHashFromFile(Hash *H){
    FILE *file = fopen("students_hash.data", "r");
    if (file == NULL) {
        printf("Cannot open students_hash.data file!\n");
        return;
    }

    char line[512];
    char name[60], major[40];
    char courseCode[20], courseTitle[80], semester[20];
    int id, creditHours;

    for (int i = 0; i < H->table_size; i++)
        H->hash_table[i].status = 0;

    H->loaded_Data = 0;

    while (fgets(line, sizeof(line), file)) {

        char *token = strtok(line, "#");
        if (!token)
        continue;
        strcpy(name, token);

        token = strtok(NULL, "#");
        if (!token)
        continue;
        id = atoi(token);

        token = strtok(NULL, "#");
        if (!token)
        continue;
        strcpy(major, token);

        token = strtok(NULL, "#"); // courseCode
        if (!token)
        continue;
        strcpy(courseCode, token);

        token = strtok(NULL, "#"); // courseTitle
        if (!token)
        continue;
        strcpy(courseTitle, token);

        token = strtok(NULL, "#"); // creditHours
        if (!token)
        continue;
        creditHours = atoi(token);

        token = strtok(NULL, "#"); // semester
        if (!token)
        continue;
        strcpy(semester, token);

        semester[strcspn(semester, "\n")] = '\0';

        InsertStudentDirectToHash(H, name, id, major);
    }

    fclose(file);
    printf("Hash table loaded from students_hash.data successfully.\n");
}


// menu hash

// 1 helper
void PrintStudentCoursesFromFile(int id){
    FILE *file = fopen("students_hash.data", "r");
    if (file == NULL) {
        printf("Cannot open students_hash.data\n");
        return;
    }

    char line[512];
    char name[60], major[40];
    char courseCode[20], courseTitle[80], semester[20];
    int fileID, creditHours;
    int found = 0;

    while (fgets(line, sizeof(line), file)) {

        char *token = strtok(line, "#");
        if (!token)
        continue;
        strcpy(name, token);

        token = strtok(NULL, "#");
        if (!token)
        continue;
        fileID = atoi(token);

        token = strtok(NULL, "#");
        if (!token)
        continue;
        strcpy(major, token);

        token = strtok(NULL, "#");
        if (!token)
        continue;
        strcpy(courseCode, token);

        token = strtok(NULL, "#");
        if (!token)
        continue;
        strcpy(courseTitle, token);

        token = strtok(NULL, "#");
        if (!token)
        continue;
        creditHours = atoi(token);

        token = strtok(NULL, "#");
        if (!token)
        continue;
        strcpy(semester, token);

        int i = 0;
        while (semester[i] != '\0') {
           if (semester[i] == '\n') {
               semester[i] = '\0';
                break;
          }
           i++;
       }

        if (fileID == id) {
            if (!found) {
                printf("Courses:");
                found = 1;
            }
            printf("%s (%s)\n", courseCode, semester);
        }
    }

    fclose(file);

    if (!found)
        printf(" No courses found.\n");
}


//case 1 work
void PrintHashTable(Hash *H){
    if (H == NULL || H->hash_table == NULL) {
        printf("Hash table not initialized!\n");
        return;
    }

    for (int i = 0; i < H->table_size; i++) {

        if (H->hash_table[i].status == 1) {
            printf("\n%d \n", i);
            printf("   Name : %s\n", H->hash_table[i].name);
            printf("   ID   : %d\n", H->hash_table[i].id);
            printf("   Major: %s\n", H->hash_table[i].major);

            PrintStudentCoursesFromFile(H->hash_table[i].id);
        }
        else if (H->hash_table[i].status == -1) {
            printf("%d) DELETED\n", i);
        }
        else {
            printf("%d) EMPTY\n", i);
        }
    }
}

//case 2
void PrintHashSize(Hash *H){
    printf("Table size: %d\n", H->table_size);
    printf("Loaded data: %d\n", H->loaded_Data);
}


//case 3
void PrintHashFunction(){
    printf("Hash Function Used:\n");
    printf("h(key) = (sum of ASCII values of characters in student name) mod TableSize\n\n");

    printf("To provied Collision :\n");
    printf("Linear Probing: h(key, i) = (h(key) + i) mod TableSize\n");
}


//CASE 4
void InsertStudentToHash(Hash *H, AVLNode root){
    int id, creditHours;
    char name[60], major[40];
    char courseCode[20], courseTitle[80], semester[20];

    printf("Enter student ID: ");
    scanf("%d", &id);

    AVLNode s = Find(id, root);

    if (s != NULL) {
        printf("Student already exists.\n");

        printf("Enter Course Code: ");
        scanf("%s", courseCode);

        if (HasCourse(courseCode, s->regs)) {
            printf("Student already registered in this course.\n");
            return;
        }

        getchar();
        printf("Enter Course Title: ");
        gets(courseTitle);

        while (1) {
            printf("Enter Credit Hours: ");
            if (scanf("%d", &creditHours) == 1)
                break;

            printf("Invalid Credit Hours! Please enter numbers only.\n");
            while (getchar() != '\n');
        }

        getchar();
        printf("Enter Semester: ");
        gets(semester);

        insertCourseLast(
            s->regs,
            createCourse(courseCode, courseTitle, creditHours, semester)
        );

        printf("Course added successfully.\n");
        return;
    }

    getchar();
    printf("Enter Student Name: ");
    gets(name);

    printf("Enter Major: ");
    gets(major);

    printf("Enter Course Code: ");
    scanf("%s", courseCode);

    getchar();
    printf("Enter Course Title: ");
    gets(courseTitle);

    while (1) {
        printf("Enter Credit Hours: ");
        if (scanf("%d", &creditHours) == 1)
            break;

        printf("Invalid Credit Hours! Please enter numbers only.\n");
        while (getchar() != '\n');
    }

    getchar();
    printf("Enter Semester: ");
    gets(semester);

    InsertReg(
        name, id, major,
        courseCode, courseTitle,
        creditHours, semester,
        root
    );

    printf("New student added successfully.\n");
}


// 5 helper
int SearchHash(Hash *H, char name[]){
    int index = HashFunction(name, H->table_size);
    int start = index;

    while (H->hash_table[index].status != 0) { //not

        if (H->hash_table[index].status == 1 &&
            strcmp(H->hash_table[index].name, name) == 0)
            return index;

        index = (index + 1) % H->table_size;

        if (index == start)
            break;
    }

    return -1;
}

// case 5
void SearchStudentInHash(Hash *H, AVLNode root)
{
    char name[60];
    int indices[50];   //to save student name
    int count = 0;
    int id;
    int foundIndex = -1;

    printf("Enter student name to search: ");
    getchar();
    gets(name);

    for (int i = 0; i < H->table_size; i++) {
        if (H->hash_table[i].status == 1 &&
            strcmp(H->hash_table[i].name, name) == 0) {

            indices[count++] = i;
        }
    }

    if (count == 0) {
        printf("Student not found.\n");
        return;
    }

    if (count == 1) { // name not repeated
        foundIndex = indices[0];
    }
    else {
        printf("More than one student found with this name.\n");
        printf("Please enter student ID: ");
        scanf("%d", &id);

        for (int i = 0; i < count; i++) {
            if (H->hash_table[indices[i]].id == id) {
                foundIndex = indices[i];
                break;
            }
        }

        if (foundIndex == -1) {
            printf("No student with this ID.\n");
            return;
        }
    }

    printf("\nStudent found:\n");
    printf("Name : %s\n", H->hash_table[foundIndex].name);
    printf("ID   : %d\n", H->hash_table[foundIndex].id);
    printf("Major: %s\n", H->hash_table[foundIndex].major);

    AVLNode s = Find(H->hash_table[foundIndex].id, root);

    if (s == NULL || s->regs == NULL || s->regs->next == NULL) {
        printf("No courses registered for this student.\n");
        return;
    }

    printf("\nRegistered Courses:\n");
    CourseNode c = s->regs->next;
    while (c != NULL) {
        printf("- %s\n", c->courseCode);
        c = c->next;
    }
}

//case 6 done
void DeleteFromHash(Hash *H, char name[])
{
    int index = SearchHash(H, name);

    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    H->hash_table[index].status = -1; // DELETED
    H->loaded_Data--;

    printf("Student deleted successfully.\n");
}


//case 7
void SaveAVLToRegFileHelper(AVLNode T, FILE *out)
{
    if (T == NULL)
        return;

    SaveAVLToRegFileHelper(T->Left, out);

    CourseNode c = T->regs->next;
    while (c != NULL) {

        fprintf(out, "%s", T->name);
        fprintf(out, "#");

        fprintf(out, "%d", T->id);
        fprintf(out, "#");

        fprintf(out, "%s", T->major);
        fprintf(out, "#");

        fprintf(out, "%s", c->courseCode);
        fprintf(out, "#");

        fprintf(out, "%s", c->courseTitle);
        fprintf(out, "#");

        fprintf(out, "%d", c->creditHours);
        fprintf(out, "#");

        fprintf(out, "%s", c->semester);
        fprintf(out, "\n");

        c = c->next;
    }

    SaveAVLToRegFileHelper(T->Right, out);
}

void SaveAVLToRegFile(AVLNode root)
{
    FILE *out = fopen("reg.txt", "w");
    if (out == NULL) {
        printf("Cannot open reg.txt file!\n");
        return;
    }

    SaveAVLToRegFileHelper(root, out);

    fclose(out);
    printf("All data saved to reg.txt successfully.\n");
}


//MAIN
void HashMenuLoop(Hash *H, AVLNode root)
{
    char choice[10];
    char name[60];

    while (1)
    {
        printf("\n==== Hash Menu ====\n");
        printf("1. Print hashed table (including empty/deleted)\n");
        printf("2. Print table size\n");
        printf("3. Print used hash function\n");
        printf("4. Insert a new record\n");
        printf("5. Search for a student\n");
        printf("6. Delete a record\n");
        printf("7. Save hash table back to file\n");
        printf("8. Back to AVL menu\n");

        printf("Enter your choice: ");
        scanf("%9s", choice);

        switch (atoi(choice))
        {
            case 1:
                PrintHashTable(H);
                break;

            case 2:
                PrintHashSize(H);
                break;

            case 3:
                PrintHashFunction();
                break;

            case 4:
                InsertStudentToHash(H, root);
                break;

            case 5:
                SearchStudentInHash(H, root);
                break;

            case 6:
                getchar();
                printf("Enter student name to delete: ");
                gets(name);
                DeleteFromHash(H, name);
                break;

            case 7:
              SaveAVLToRegFile(root);
                break;

            case 8:
                printf("Returning to AVL menu...\n");
                return;

            default:
                printf("Invalid choice!\n");
        }
    }
}

