#include<stdio.h>
#include<stdlib.h>
#include<time.h>

/*Stores basic information about the current status and properties of an elevator*/
struct elevator{
	int max_floor;
	int cur_floor;
	int max_cap;
	int cur_cap;
	int lock;	//If lock = 1, lift will stop operating else lift continues to operate
	int dir;
};

struct 	node{
	int floor;
	int x; // x=0 means entry request and x = 1 means drop reuqest
	int c; //The number of people at that floor to be dropped
	struct node * next;
};

int key; //For storing the elevator's password

/* To delay the execution by sec time*/
void wait(int sec){
	time_t start, end;
	time(&start);
	
	do time(&end); 
	while(difftime(end, start) <= sec);
}

/*Used to get the number of entry requests i.e., 
the floors from where the button is being pressed(outside) 
It is fed into an array
*/
int lift_entry(struct elevator * lift, int * a)
{
	int num;
	printf("Enter the number of entry requests: ");
	scanf("%d", &num);
	if(num!=0)
	{
		printf("Enter the floors from where the entry request is being made: \n");
		for(int i=0; i<num; i++)
		{
			scanf("%d", &a[i]);
			while(!(a[i]>=0 && a[i]<=(lift)->max_floor))
			{
				printf("Enter a valid entry floor: \n");
				scanf("%d", &a[i]);
			}	
		}
	}
	return num;
}

/* Used to search if the same floor with similar x(entry or drop) is present*/
int search(struct node ** top, int item, int x) //Returns 0 if item not found else returns 1
{
	if((*top) == NULL)
	{
		return 0;
	}
	struct node * temp = *top;
	while(temp)
	{
		if(temp->floor == item)
		{
			if(temp->x == 1 && x == 0) 
			{
				//There's a node with drop request at that floor
				//We are getting an entry request to the same floor
				return 1;
			}
			if(temp->x == 0 && x == 1)
			{
				//There's a node with entry request at that floor
				//We are getting a drop reuqest to the same floor
				temp->x = 1;
				temp->c = 1;
				return 1;
			}
			if(temp->x ==1)
			{
				//In case a drop request is already present
				temp->c++;
				printf("%d", temp->c);
			}
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}

/* Displays the upward or downward linked list*/
void display(struct node *  top)
{
	if(top == NULL)
	{
		printf("Empty\n");
		return;
	}
	struct node * temp = top;
	while(temp)
	{
		printf("\nx = 0 means entry request, x = 1 means drop request.\n");
		printf("[ flr = %d, x = %d ", temp->floor, temp->x);
		if(temp->x==1)
		{
			printf(", c = %d]  ->  ", temp->c);
		}
		else
		{
			printf("]  ->  ");
		}
		temp = temp->next;
	}
	printf("NULL\n");
}

int NullCheck(void* ptr)
{
    if(ptr==NULL)
    {
        printf("Memory Error\n");
        exit(0);
    }
}

struct node * create(int flr, int x)
{
	struct node * newnode = NULL;
	newnode = (struct node *)malloc(sizeof(struct node));
	NullCheck(newnode);
	newnode->floor = flr;
	newnode->x = x;
	// x = 0 means pick up; x = 1 means drop from lift
	if(x==1)
	{
		newnode->c = 1;
	} 
	newnode->next = NULL;
	return newnode;
}

void insert_floor(int * a, int num, int x, struct elevator ** lift, struct node ** up_list_top, struct node ** down_list_top)
{
	int i=0, s;
	while(i<num)
	{
		struct node * newnode = create(a[i], x);
		
		//First two checks confirm the cases where the up and down queues are empty
		if(*up_list_top == NULL)
		{	
			
			if(a[i] > (*lift)->cur_floor)
			{
				*up_list_top = newnode;
				i++;
				continue;
			}
		}
			
		if(*down_list_top == NULL)
		{
			if(a[i] < ((*lift)->cur_floor))
			{
				*down_list_top = newnode;
				i++;
				continue;
			}
		}
			
		//Now we have the cases where we need to insert the floor in up or down queue when not empty
		if(a[i] > (*lift)->cur_floor)
		{
			struct node * temp = *up_list_top;
			struct node * prev = temp;
			
			/*
			The floor to be added(could be entry or leave request) is above the current floor
			Check if a duplicate value is already present 
			In search(), if x==1(drop request) and a similar floor is present, increase c by 1
			Because while dropping the people from a particular floor
			We need to make sure that lift->cur_cap is accordingly updated
			*/ 
			s = search(up_list_top, a[i], x);
			if(s == 1)
			{
				i++;
				continue;
			}
			
			if(newnode->floor < (*up_list_top)->floor)
			{
				//Insertion in beginning
				//Upward queue gets sorted in ascending order
				newnode->next = *up_list_top;
				(*up_list_top) = newnode;
			}
			else
			{
				//Insertion in intermediary or end in asc order
				
				while((newnode->floor > temp->floor) && temp->next!=NULL)
				{	
					prev = temp;
					temp = temp->next;
				}
				
				if(temp->next == NULL)
				{
					if(newnode->floor < temp->floor)
					{
						prev->next = newnode;
						prev->next->next = temp;
					}
					else
					{
						temp->next = newnode;
						temp->next->next = NULL;
					}
				}
				else
				{
					prev->next = newnode;
					prev->next->next = temp;
				} 
			} 
			i++;
		}
		else
		{
			//The floor to be added(could be entry or leave request) is above the current floor
			struct node * temp = *down_list_top;
			struct node * prev = temp;
			
			s = search(down_list_top, a[i], x);
			if(s == 1)
			{
				i++;
				continue;
			}
			
			if(newnode->floor > (*down_list_top)->floor)
			{
				//Insertion in beginning
				//Downward queue gets sorted in descending order
				newnode->next = (*down_list_top);
				*down_list_top = newnode;
			}
			else
			{
				//Insertion in intermediary or end in desc order
				struct node * temp = *down_list_top;
				struct node * prev = temp;
				
				while((newnode->floor < temp->floor) && temp->next!=NULL)
				{	
					prev = temp;
					temp = temp->next;
				}
		
				if(newnode->floor < temp->floor)
				{
					temp->next = newnode;
					temp->next->next = NULL;	
				}
				else
				{
					prev->next = newnode;
					prev->next->next = temp;
				}
			} 
			i++;
		}
		
	}
}

int e_request_after_pick(struct elevator * lift, struct node * temp, int * a)
{
	int i, count;
	printf("How many people want to enter? :\n");
	scanf("%d", &count);
	while(count >  lift->max_cap - lift->cur_cap)
	{
			printf("--------------OVERLOAD!!-------------\n");
			printf("\a");
			
			printf("Only %d people can enter\n", lift->max_cap - lift->cur_cap);
			count = lift->max_cap - lift->cur_cap;
	}
	if(count != 0)
	{
		printf("\nEnter drop floor for %d requests: ", count);
		for(i=0; i<count; i++)
		{
			scanf("%d", &a[i]);
			while(!(a[i]>=0 && (a[i]<=(lift)->max_floor) && a[i]!=temp->floor))
			{
				printf("Enter a valid drop floor: \n");
				scanf("%d", &a[i]);
			}
		}
	}
	return count;
}

/*The function makes the lift move up or down by removing the elements
from upward or downward linked list*/
void move(struct elevator ** lift, struct node ** up_list_top, struct node ** down_list_top, int dir)
{
	int i;
	int count;
	int * a;
	a = (int *)malloc(sizeof(int) * (*lift)->max_cap);
	struct node * temp;
	
	if(dir == 1)
	{
		if(*up_list_top == NULL)
		{
			(*lift)->dir = 2;
			return;
		}
		printf("\n\n\n\n-----------LIFT MOVING UP-----------\n");
		temp = *up_list_top;
		*up_list_top = (*up_list_top)->next;
	}
	else if(dir == 2)
	{
		if(*down_list_top == NULL)
		{
			(*lift)->dir = 1;
			return;
		}
		printf("\n-----------LIFT MOVING DOWN-----------\n");
		temp = *down_list_top;
		*down_list_top = (*down_list_top)->next;
	}
	
	if((*lift)->cur_cap !=0)
	{
		printf("FAN : ON \t\t LIGHT: ON\n");
	}
	else
	{
		printf("FAN : OFF \t\t LIGHT: OFF\n");
	}
	
	printf("No. of people currently in the lift: %d\n\n", (*lift)->cur_cap);
	wait(3);
	printf("Lift now at FLOOR %d\n", temp->floor);
	
	if(temp->x == 0)
	{
		printf("SATISFIED REQUEST for ENTRY at floor %d\n", temp->floor);
		(*lift)->cur_floor = temp->floor;
		count = e_request_after_pick(*lift, temp, a);
		for(i=0; i<count; i++)
		{
			(*lift)->cur_cap++;
		}
		insert_floor(a, count, 1, lift, up_list_top, down_list_top);
	}
	else if(temp->x == 1)
	{
		printf("SATISFIED REQUEST for DROP at FLOOR %d\n", temp->floor);
		for(i=0; i < temp->c; i++)
		{
			(*lift)->cur_cap--;
		}
		printf("No. of people currently in the lift after drop: %d\n\n", (*lift)->cur_cap);
		(*lift)->cur_floor = temp->floor;
		printf("Calling for request from passengers at FLOOR %d \n", temp->floor);
		int num = lift_entry(*lift, a);
		if(num!=0)
		{
			insert_floor(a, num, 0, lift, up_list_top, down_list_top);
		}
	}	
} 

void delete(struct node ** ptr)
{
	struct node * temp = NULL;
	while(*ptr)
	{
		temp = (*ptr)->next;
		free(*ptr);
		*ptr = NULL;
		*ptr = temp;
	}
}

int main()
{	
	int ps, ch=0;
	struct node * up_list_top = NULL;
	struct node * down_list_top = NULL;
	struct elevator * lift = NULL;
	lift = (struct elevator *)malloc(sizeof(struct elevator));
	if(!lift)
	{
		printf("Memory not allocated\n");
		return(0);
	}
	int num;
	
	printf("Enter the maximum number of floors: \n");
	scanf("%d", &lift->max_floor);
	lift->cur_floor = 0;
	
	printf("Enter the maximum capacity of the lift: \n");
	scanf("%d", &lift->max_cap);
	
	lift->lock = 0;
	printf("\nYou need to enter a password so that when ");
	printf("the lift's use is over, you can stop it. ");
	printf("Otherwise the lift will keep working or waiting for next request.\n\n");
	printf("Enter a lock password: \n");
	scanf("%d", &key);
	
	lift->cur_cap = 0;
	int * a;
	a = (int *)malloc(sizeof(int) * lift->max_cap);
	
	//lift->dir represents direction of lift --> 1 - up; 2- down
	lift->dir = 1;
	
	printf("Lift now at FLOOR 0\n");num = lift_entry(lift, a);
	
	insert_floor(a, num, 0, &lift, &up_list_top, &down_list_top);
	display(up_list_top);
	//display(down_list_top);
	
	while(1)
	{
		ch = 0;
		if(!up_list_top && !down_list_top)
		{
			while(ch!=1 && ch!=2)
			{
				printf("\n\nThere are no requests now\n");
				printf("1. Place new entry request\n");
				printf("2. Enter lift's lock password\n");
				printf("Enter your choice (1 or 2)");
				scanf("%d", &ch);
				if(ch==1)
				{
					num = lift_entry(lift, a);
					insert_floor(a, num, 0, &lift, &up_list_top, &down_list_top);
					display(up_list_top);
					display(down_list_top);
				}
				else if(ch==2)
				{
					printf("Enter the password now: \n");
					scanf("%d", &ps);
					if(ps == key)
					{
						lift->lock = 1;
						printf("\n\n--------Thank you for using the lift--------\n\n");
						break;
					}
				}
				else
				{
					printf("Invalid choice, enter only 1 or 2\n\n");
				}
			}
		}
		if(lift->lock == 1)
		{
			break;
		}	
		if(lift->dir == 1)
		{
			move(&lift, &up_list_top, &down_list_top, lift->dir);
		}
	
		if(lift->dir == 2)
		{
			move(&lift, &up_list_top, &down_list_top, lift->dir);
		}
		printf("\n\n");
		printf("------------------\n");
		printf("Upward Linked List\n");
		display(up_list_top);
		printf("Downward Linked List\n");
		display(down_list_top);
		printf("------------------\n");
		printf("\n\n");
	}
	delete(&up_list_top);
	delete(&down_list_top);
	free(lift);
	return(0);
}
