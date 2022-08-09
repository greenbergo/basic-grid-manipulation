// Author   Ofer Greenberg
// Date     May 31st, 2022

// Purpose: 	Take in a matrix of characters and perform 
//		clockwise and counter clockwise rotations,
//		horizontal and vertical shifts and inversions
//		

// Pseudo:
// If user included test file
//	open file
// Else 	
//	prompt user for input from stdin
// Then     -> Read input into an array
//          -> Prompt user to choose an operation
//          -> Print result


#include <iostream>
#include <cstring>
#include <cctype>
#include <fstream>
#include <cmath>

#define MAX_INT 2147483647

using namespace std;


// -- Structs --
typedef struct grid {
    int height;
    int width;
    int hshift = 0;
    int vshift = 0;
    float rotation = 0; 
    char *matrix;
} Grid;


// -- Prototypes --
void loadInput(istream& in, Grid* input);
bool executeOperation(char op[], Grid* input);
void rotate(Grid* grid, char* copy, float degrees);
int getRotatedIndex(int x, int y, float sin_val, float cos_val, Grid* grid);
void vmirror(Grid* grid);
void hmirror(Grid* grid); 
void printCommandMenu();
bool isEqual(char strLeft[], const char strRight[]);
void prettyPrint(char matrix[], int rows, int cols);
void prettyPrint(Grid* grid);
void copyCharArray(const char arr[], char *copy, int size);
void printShiftedRows(int vshift);
void printShiftedCols(int hshift);
void freeArray(Grid* grid);


// Take user input
// Perform user operations
// Print results to stdout
int main(int argc, char *argv[]) {
    Grid* input;
    char op[8];
    bool isDone = false;
    ifstream inputFile;

    // Open input file
    if (argc > 1) {
        inputFile.open(argv[1],ios::in);
    } 

    // load input from file
    if (inputFile.is_open()) {
        loadInput(inputFile, input);
        inputFile.close(); 

    // load input from stdin
    } else {
        cout << "Error: input file wasn't detected\n";
        cout << "Insert input: [rows] [cols] val1 val2 val3... "; 
        loadInput(std::cin, input);
    }

    // perform operations on matrix
    while (!isDone){
        cout << "\nEnter Operation:"; // other inputs include, exit and help
        cin >> op;
       
        if (isEqual(op, "exit")){
            isDone = true;
        } else {
            executeOperation(op, input);
        }
    }

    free(input->matrix); // TODO - test with valgrind
}

// -------------------- |
// -- Core Functions -- |
// -------------------- |

// Reads input file into a grid struct
void loadInput(istream& in, Grid* output) { // (std::istream& in)
    char temp[2];

    // get dimentions
    int rows = 0;
    int cols = 0;
    int m;
    in >> rows >> cols;
    
    // set-up grid
    m = max(rows,cols);
    if (m % 2 == 0) { // rotation works best with odd number of cols and rows
        m++;
    }
    output->height = m;
    output->width = m;
    output->hshift = 4;
    output->vshift = 4;
    output->matrix = (char*) malloc (sizeof(char)*(m*m+1)); 

    // read space seperated char values
    // or first char of a word if format is wrong
    // fill rest matrix with spaces
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < m; j++) {
            if (j < cols && i < rows){
                in >> temp;
                output->matrix[i*m+j] = temp[0];  
            } else {
                output->matrix[i*m+j] = ' ';
            }
        }
    }
    output->matrix[m*m+1];
    
    cout << "rows: " << rows << "  cols: " << cols << "\n";
    prettyPrint(output);
}

// checks which operation the user entered 
// calls appropriate function and print grid
// **Perhapse switch-case would be a little more efficient here
// **experimented with Ternary Operator for fun
bool executeOperation(char op[], Grid* input) {
    float degrees = 0;
    unsigned int shift = 0;

    if (isEqual(op, "rotate")) { 
        cin >> degrees;
        input->rotation = input->rotation + degrees;
    } else if (isEqual(op, "crotate")) { // counter clockwise
        cin >> degrees;
        input->rotation = input->rotation - degrees;
    } else if (isEqual(op, "hmirror")) {
        hmirror(input);
    } else if (isEqual(op, "vmirror")) { 
        vmirror(input);
    
    //   Shifting Grid
    } else if(isEqual(op, "up")) {
        cin >> shift;
        input->vshift = max((int)(input->vshift-shift), 0);
    } else if(isEqual(op, "down")) {
        cin >> shift;
        input->vshift = ((input->vshift+shift)*4 + (input->height) < 101) ? input->vshift+shift : (100-input->height)/4;
    } else if(isEqual(op, "left")) {
        cin >> shift;
        input->hshift = max((int)(input->hshift-shift), 0);
    } else if(isEqual(op, "right")) {
        cin >> shift;
        input->hshift = ((input->hshift+shift)*4 + (input->width) < 121) ? input->hshift+shift : (120-input->height)/4;
    } else {
        printCommandMenu();
    }

    prettyPrint(input);

    // flash cin buffer in case of input error
    cin.clear();
    cin.ignore(MAX_INT, '\n');
}

// perform rotation on given matrix
// change is not perform on the original
void rotate(Grid* grid, char* copy, float degrees) {
    float radians = (degrees * M_PI / 180.0);
    int cols = grid->width;
    int rows = grid->height;

    // setup
    float sin_val = sin(radians);
    float cos_val = cos(radians);
    int k = 0; // rotated index

    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            k = getRotatedIndex(j, i, sin_val, cos_val, grid);
            if (k < 0){
                copy[i*cols + j] = ' ';
            } else {
                copy[i*cols + j] = grid->matrix[k];
            }
            
        }
    }
    cout << "\n";
}

// get shifted index from rotation 
// using (x cos ϕ−y sin ϕ, x sin ϕ+y cos ϕ)
//      1. transform index to center
//      2. use rotation formula
//      3. bring back to original location
//      4. check bounds
//
int getRotatedIndex(int x, int y, float sin_val, float cos_val, Grid* grid) {
    float x_center = grid->width/2;
    float y_center = grid->height/2;

    // simulating shifting center to origin
    float x_delta = x - x_center;
    float y_delta = y_center - y; 

    // getting coordinates of rotated pixel
    x = round((x_delta * cos_val) - (y_delta * sin_val)); // *(-1)
    y = round((x_delta * sin_val) + (y_delta * cos_val));

  
    // check bound - maybe uneccesary 
    if ((x + x_center < grid->width &&  x + x_center >= 0) 
        && (y_center - y < grid->height &&  y_center - y >= 0)) {

        return round(grid->width * ((y_center) - y) + (x_center + x)); // y index starts on 'top'
    }    
    return -1; // used as flag
}

// perform vertical mirror transformation on given matrix
void vmirror(Grid* grid) {
    int cols = grid->width;
    int rows = grid->height;
    char copy[cols*rows+1];
    copyCharArray(grid->matrix, copy, cols*rows);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid->matrix[(rows - i - 1)*cols + j] = copy[i*cols + j]; // 2d coordinates for 1d array
        }
    }
    cout << "\n";
}

// perform horizontal mirror transformation on given matrix
void hmirror(Grid* grid) {
    int cols = grid->width;
    int rows = grid->height;
    char copy[cols*rows+1];
    copyCharArray(grid->matrix, copy, cols*rows);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid->matrix[i*cols + (cols - j - 1)] = copy[i*cols + j]; // 2d coordinates for 1d array
        }
    }
    cout << "\n";
}


// prints menu of legal matrix operations to perform and their format
void printCommandMenu() {
    cout << "\n\n";
    cout << "Legal Operations:\n(1) rotate [int]\n(2) crotate [int]\n(3) vmirror\n(4) hmirror\n(5) exit\n";
}


// ----------------------- |
// -- Utility Functions -- |
// ----------------------- |

// return true if two strings are equal (CASE-INSENSATIVE)
bool isEqual(char strLeft[], const char strRight[]) {
    int n = strlen(strLeft);
    if (n != strlen(strRight)){
        return false;
    }

    for (int i = 0; i < n; i++) {
        if (strLeft[i] != strRight[i]) {

            // convert one string's char to lower case (if it is upper)
            // recheck if the two chars are now matching 
            if (strLeft[i] >= 'A' && strLeft[i] <= 'Z'){
                strLeft[i] = strLeft[i] - 'A' + 'a';
            } 
            
            if (strLeft[i] != strRight[i]) {
                return false;
            }
        }
    }
    return true;
}


// Prints Matrix 
void prettyPrint(Grid* grid){
    int rows = grid->height;
    int cols = grid->width;
    char matrix[rows*cols+1];
    copyCharArray(grid->matrix,matrix,rows*cols);
    rotate(grid, matrix, grid->rotation);

    char temp;
    bool emptyRow = true;
    string temp_str = "";

    // print only neceessary cells and rows
    cout << "shifted: " << grid->hshift << ", " << grid->vshift << "\n"; // -- testing line --
    cout << "rotateded: " << grid->rotation << "\n"; // -- testing line --
    printShiftedRows(grid->vshift);
    for (int i = 0; i < rows; i++) {
        printShiftedCols(grid->hshift);
        for (int j = 0; j < cols; j++) {
            
            temp = matrix[i*cols + j]; // 2d coordinates for 1d array
            if (emptyRow && temp != ' '){
                cout << temp_str << " " << temp;
                emptyRow = false;
            } else if(temp != ' '){
                cout << " " << temp;
            } else {
                temp_str = temp_str + "  ";
            }
        }

        if (!emptyRow){
            cout << "\n"; 
        }  
        emptyRow = true;
        temp_str = "";
    }
}

// returns a copy of arr with null terminator
void copyCharArray(const char arr[], char *copy, int size){
    for (int i =  0; i < size; i++){
        *(copy+i) = arr[i];
    }
    *(copy+size) = '\0';
}

// print number of empty rows representing vertical shift of the grid
void printShiftedRows(int vshift){
    while (vshift > 0){
        cout << '\n';
        vshift--;
    }
}

// print number of empty cols representing horizontal shift of the grid
void printShiftedCols(int hshift) {
    while (hshift > 0){
        cout << "    ";
        hshift--;
    }
}