// This program dispaly a robot breaking the maze and find the path to the end

#include "graphics.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define PI acos(-1.0)
#define Wall 0
#define Road 1
#define End 2
#define Temp_road 7
#define right 1
#define down 2
#define left 3
#define up 4

const int maze_size = 29; // number of square a row and a coloum, must be odd;
const int window_size = 900;
const int square_size = (int)(window_size / maze_size + 0.5);

// initialize in the main function
int start_x = 0, start_y = 1; // start coordinate;
double start_dir = 0; // start direction in radian;
int end_x = maze_size - 1, end_y = maze_size - 2; // end coordinate;
const int line_width = 1; // width of side of suqare in maze;

// covert double number to integer, eg. 4.6 -> 5, 4.3 -> 4, -4.6 -> -5;
int double2int(double num) 
{
    if(num >= 0)
        return (int)(num + 0.5);
    else
        return (int)(num - 0.5);
}

// defining single cell;
typedef struct Cells
{
    int x, y, visited, flag; // for a specific cell, x,y are coordiantes, visited represents it has been visited(1) or not(0), flag marked its type Wall, Road, End;
    
}cell;

// initialize a specific cell
void init_cell(cell *acell, int x, int y, int visited, int flag)
{
    acell->x = x;
    acell->y = y;
    acell->visited = visited;
    acell->flag = flag;
}

// draw a specific cell
void draw_cell(cell *acell)
{
    setColour(black);
    // calculate actual coordinates
    int coor_x = acell->x * square_size;
    int coor_y = acell->y * square_size;
    drawRect(coor_x, coor_y, square_size, square_size);
    switch (acell->flag)
    {
    case Wall:
        setColour(black);
        break;
    case Road: 
        setColour(white);
        break;
    case End:
        setColour(blue);
        break;
    default:
        break;
    };
    fillRect(coor_x + line_width, coor_y + line_width , square_size - line_width, square_size - line_width); // the line width is to show sides of cell;
    setColour(red); // set color to start
    fillRect(start_x * square_size + line_width, start_y * square_size + line_width , square_size - line_width, square_size - line_width);
}

// defining stack 
// Creating a stack
typedef struct CellStacks
{
    cell route[(maze_size - 1) * (maze_size - 1) + 2]; // the longest path may be going through all the cell inside the wall plus start and end.
    int top;
    int count;
}cellstack;

void createEmptyStack(cellstack *s) {
    s->top = -1; // first element index at top
    s->count = 0; // number of elements
}

// Check if the stack is full
int isfull(cellstack *s) {
    if (s->top == maze_size * maze_size - 1)
        return 1;
    else
        return 0;
}

// Check if the stack is empty
int isempty(cellstack *s) {
    if (s->top == -1 || s->count == 0)
        return 1;
    else
        return 0;
}

// Add elements into stack
void push(cellstack *s, cell new_cell) 
{
    if (isfull(s)) {
        printf("STACK FULL");
    } else {
        s->top++;
        s->route[s->top] = new_cell;
    }
    s->count ++;
}

// Remove element from stack
void pop(cellstack *s) {
    if (isempty(s))
        printf("\n STACK EMPTY \n No solution avaiable\n");
    else 
        s->top--;
    s->count--;
  printf("\n");
}

cellstack available_road; // define maze road;
cell maze[maze_size][maze_size]; // defining maze array using cell type; 

// decide whether the point is inside the maze  surrounded by the walls
int inArea(int x, int y)
{
	if(x > 0 && x < maze_size - 1 && y > 0 && y < maze_size - 1)
		return 1;
	return 0;
}

// desiede whether there are roads that are not visited arund a specific point.
int have_neighbor(int x, int y)
{
	for (int i = x - 2; i <= x + 2; i+= 2)
		for (int j = y - 2; j <= y + 2; j+= 2)
			if (abs(x - i) + abs(y - j) == 2 && inArea(i, j) && maze[i][j].flag == Road)
				return 1;
	return 0;
				
}
// generate maze using dfs idea
void generate_maze_dfs()
{
	int current_x, current_y;
    for(int i = 0; i < maze_size; i ++)
        for(int j = 0; j < maze_size; j ++)
            init_cell(&maze[i][j], i, j, 0, Wall); // x, y, visited, flag

    // mark the maze[odd][odd] as wall
	for (int i = 1; i <= maze_size - 2; i += 2)
        for (int j = 1; j <= maze_size - 2; j += 2)
           	maze[i][j].flag = Road;

    // mark the start and end as road at first so that they would not affect the have_neighbor function
	maze[start_x][start_y].flag = Road;
	maze[end_x][end_y].flag = Road;
	createEmptyStack(&available_road);

    // start drawing maze from the point next to the start point
	push(&available_road, maze[start_x + 1][start_y]);
    // there are unused roads in stack
	while(!isempty(&available_road))
	{
        // take the top road to extend
		cell top_cell = available_road.route[available_road.top];
		current_x = top_cell.x;
		current_y = top_cell.y;
		pop(&available_road);

        // current point could connect to adjacent unvisited road
		while(have_neighbor(current_x, current_y))
		{
            // random 4 directions
			int rand_dir = rand() % 4; 
			int dx = double2int(cos(rand_dir * PI / 2));
			int dy = double2int(sin(rand_dir * PI / 2));
            // the target road is in available range
			if(inArea(current_x + 2 * dx, current_y + 2 * dy) && maze[current_x + 2 * dx][current_y + 2 * dy].flag == Road)
			{
                // mark the cell gone through as visited
				maze[current_x + dx][current_y + dy].flag = Temp_road;
				maze[current_x + 2 * dx][current_y + 2 * dy].flag = Temp_road;
                // push new cell into stack
				push(&available_road, maze[current_x + 2 * dx][current_y + 2 * dy]);
			}
		}
	}
    // convert all visited temporary raods to roads
	for (int i = 1; i <= maze_size - 2; i ++)
        for (int j = 1; j <= maze_size - 2; j ++)
           	if(maze[i][j].flag == Temp_road)
				maze[i][j].flag = Road;
    // mark end 
    maze[end_x][end_y].flag = End;
}

//draw the maze 
void draw_maze()
{
    // on the background canvas
    background(); 
    for(int i = 0; i < maze_size; i ++)
        for(int j = 0; j < maze_size; j ++)
        {
            draw_cell(&maze[i][j]);
        }
}


// defining robot;
typedef struct Robots
{
    int x, y; // location of robot defioned as the element in the int maze matrix;
    double direction; // direction defined as rotated angle in Cartesian coordinate system, right is 0 radian;
}robot;

void draw_robot(robot *arobot);

// initialize the robot, including draw it;
void init_robot(robot *arobot, int init_x, int init_y, double dir)
{
    arobot->x = init_x;
    arobot->y = init_y;
    arobot->direction = dir;
    draw_robot(arobot);
}

//draw a robot;
void draw_robot(robot *arobot)
{
    // draw on foreground
    foreground();
    // set robot color to green
    setColour(green); 
    // radius of circumscribed circle of our robot, -5 is to adjust its size
    double radius = sqrt(2 * pow((double)(square_size/2), 2.0)) - 5;
    // the actual coordinate of the center of cell
    double central_x = (arobot->x + 0.5) * square_size;
    double central_y = (arobot->y + 0.5) * square_size;
    // center of circumscribed circle of our robot
    double rotation_center_x = central_x + cos(arobot->direction + PI)*square_size/6;
    double rotation_center_y = central_y - sin(arobot->direction + PI)*square_size/6;
    // calculate every vertex of triangle, the sin and cos could help the robot to point to the moving direction
    int x1 = rotation_center_x + radius * cos(arobot->direction);
    int x2 = rotation_center_x + radius * cos(arobot->direction + 2*PI/3);
    int x3 = rotation_center_x + radius * cos(arobot->direction + 4*PI/3);
    int y1 = rotation_center_y - radius * sin(arobot->direction);
    int y2 = rotation_center_y - radius * sin(arobot->direction + 2*PI/3);
    int y3 = rotation_center_y - radius * sin(arobot->direction + 4*PI/3);
    // conbine to x and y array to draw
    int robot_x[3] = {x1, x2, x3};
    int robot_y[3] = {y1, y2, y3};
    fillPolygon(3, robot_x, robot_y);
    sleep(1000 / maze_size); // so that the bigger the maze is, the faster the robot moves
    clear();
}

// defining movement of robot
void turn_right(robot *arobot)

{  
    arobot->direction = arobot->direction - PI/2;
    draw_robot(arobot);
}

void turn_left(robot *arobot)
{
    arobot->direction = arobot->direction + PI/2;
    draw_robot(arobot);
}

int can_move_forward(robot *arobot)
{
    // calculate next point according to current moving direction
    int next_x = double2int(arobot->x + cos(arobot->direction));
    int next_y = double2int(arobot->y - sin(arobot->direction));
    // if next point is not a wall and it has not been visited;
    if(maze[next_x][next_y].flag != Wall && maze[next_x][next_y].visited == 0)
        return 1; // can move
    return 0; // can not move
}

void move_forward(robot *arobot)
{
    // add moving distance in current direction;
    // since PI has been define artifically as double, the calculation precision is limited, if use (int)(a) or ceil(a) would get wrong ans;
    arobot->x += double2int(cos(arobot->direction));
    arobot->y -= double2int(sin(arobot->direction));
    maze[arobot->x][arobot->y].visited = 1;
    draw_robot(arobot);
}

int at_marker(robot *arobot);

// robot moving algorithm, using DFS idea
// reference: https://medium.com/swlh/solving-mazes-with-depth-first-search-e315771317ae#:~:text=In%20order%20to%20figure%20out,has%20nowhere%20else%20to%20go.
cellstack move_route; // define robot moving route;
void move_robot(robot *arobot)
{
    // store the cell on the path;
    createEmptyStack(&move_route); 
    // push starting point in to stack; 
    push(&move_route, maze[start_x][start_y]);
    while(!at_marker(arobot))
    {
        if(can_move_forward(arobot))
        {
            // move;
            move_forward(arobot);
            push(&move_route, maze[arobot->x][arobot->y]);
        }
        else
        {
            // check whether there is road on the rest 3 directions;
            int can_turn = 0;
            for (int i = 0; i < 3; i ++)
            {
                turn_right(arobot);
                if(can_move_forward(arobot))
                {
                    can_turn = 1; // find a way can move and not been visited;
                    break;
                }   
            }
            if(can_turn == 1)
            {
                // move;
                move_forward(arobot);
                push(&move_route, maze[arobot->x][arobot->y]);
            }
            // dead end;
            else 
            {
                // mark current cell as wall since it has no possibility to get to the end
                maze[arobot->x][arobot->y].flag = Wall; 
                // calculat next point
                int next_x = double2int(arobot->x + cos(arobot->direction));
                int next_y = double2int(arobot->y - sin(arobot->direction));
                // keep turn until it can moving back to the last drok in the road;
                while(maze[next_x][next_y].flag == Wall)
                {
                    turn_left(arobot);
                    next_x = double2int(arobot->x + cos(arobot->direction));
                    next_y = double2int(arobot->y - sin(arobot->direction));
                }
                move_forward(arobot);
                // pop the current cell out of the route;
                pop(&move_route);
            }
        }
    }
}


// check whether robots at the end;
int at_marker(robot *arobot)
{
    if (maze[arobot->x][arobot->y].flag == End)
        return 1;
    return 0;
}

void mark_route()
{
    setColour(yellow);
    for(int i = 0; i <= move_route.top; i ++)
        fillRect(move_route.route[i].x * square_size + line_width, move_route.route[i].y * square_size +line_width, square_size - line_width, square_size - line_width);
}


int main()
{   
    robot arobot;
    setWindowSize(window_size, window_size);
    generate_maze_dfs();
    draw_maze();
    printf("initializing...");
    sleep(5000);
    init_robot(&arobot, start_x, start_y, start_dir);
    move_robot(&arobot);
    mark_route();
    return 0;
}