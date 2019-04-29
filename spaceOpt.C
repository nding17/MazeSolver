#include <stdio.h>

/* avoid 'magic' numbers:
   hash define all the types of the room as indicated */
#define UNUSED   0
#define HALLWAY  1
#define BEDROOM  2
#define LIVING   3
#define BATHROOM 4
#define KITCHEN  5
#define LAUNDRY  6
#define STORAGE  7
#define GARAGE   8
#define BALCONY  9

/* 100 apartments at most, with a max of 100 rooms in each apartment, each room
   needs 4 values of info, so roughly 4*100*100 = 40000 should be the max size
   of the large array */
#define LARGE 50000

/* define a value of length for individual array, which doesn't need to be
   as large as the array that stores everything */
#define SMALL 1000

/* since the text file indicates that each row lines up with 4 numbers,
   I would define 1 cycle as equal to 4 to avoid using 'magic' number */
#define CYCLE 4

/* formatting for stage four */
#define DIV "+-------+-----------------+-----------------+-----------------+"
#define HDR "| Apart |    Dry areas    |    Wet areas    |  Utility areas  |"

/* a list of prototypes for all the functions */
int input_processing(double* input);
int search_new_apart(double* input, int start, int len);
char* roomtype_identifier(int roomtype);
void print_apart_details(double* input, int input_len);
void save_report_info(double* input, int input_len, double* added, int st);
void print_report(double* sub, int len);

/* define the types of array with
   specific length */
typedef double largeArray_t[LARGE];
typedef double smallArray_t[SMALL];

/* main function binds it all together */
int
main(int argc, char* argv[]){

	/* input is an array that stores everythinng in the
	text file, including all those -1's */
	largeArray_t input;

	/* this is an array that stores all the summary details
	in sequence, every apartment's summary details get
	incremented by 7 */
	smallArray_t summary_info;

	/* count the number of elements get stored in
	summary_info */
	int info_count = 0;

	/* the length of the large array which stores everything */
	int input_len = input_processing(input);

	   /* temporary starting and ending points to keep track of
	      the position of the current apartment */
	   int temp_start = 0;
	   int temp_end = 0;

	   /* stage 3: loop through all the apartments */
	   while(temp_end < (input_len-1)){
	   	   temp_end = search_new_apart(input,temp_start,input_len);
	   	   smallArray_t individual;
	   	   int input_count = temp_start;
	   	   int assigned = 0;

	   	   for(; input_count<temp_end; input_count++){
	   	   	   individual[assigned++] = input[input_count];
	   	   }

	   	   /* stage 1&2: print apartment details */
	   	   print_apart_details(individual, assigned);
	   	   save_report_info(individual, assigned, summary_info, info_count);
	   	   info_count += 7;
	   	   temp_start = temp_end+1;
	   	   printf("\n");
	   }

	/* stage 4: print summary details */
	print_report(summary_info, info_count);

	/* done! */
	return 0;

}


/* formatting apartment details */
void
print_apart_details(double* input, int input_len){

	int count = 1;
	double room_total = 0.0;
	double combined_area = 0.0;

	printf("Apartment %d\n", (int)(*input));
	printf("-------------\n");

	while(count <= input_len-1){
		char* room_type = roomtype_identifier((int)(*(input+count)));
		int room_num = (int)(*(input+count+1));
		double room_len = *(input+count+2);
		double room_width = *(input+count+3);
		double room_area = room_len*room_width;

		/* this is a flag of whether or not the previous rooms
		should be combined. */
		int combined = ((*(input+count)==*(input+count+CYCLE))&&
			(*(input+count+1)==*(input+count+1+CYCLE)));

		room_total += room_area;

		if(combined){
			combined_area += room_area;
			printf("   %-8.8s   %d%8.2lf%8.2lf\t     ---\n",
				room_type, room_num, room_len, room_width);
		}else{
			printf("   %-8.8s   %d%8.2lf%8.2lf\t%9.2lf\n",
				room_type, room_num, room_len, room_width,
				room_area+combined_area);

			/* to avoid unnecessary adding up of previous area */
			combined_area = 0.0;
		}
		count += CYCLE;
	}

	/* the max total area in this case theoretically possible is
	   99999.99 m^2*/
	printf("   Total area   %25.2lf metres^2\n", room_total);
	return;

}/* handle extreme cases */


/* return the position of the closest -1 in the array passed in,
   right after the starting position */
int
search_new_apart(double* input, int start, int len){
	int count = start;
	while(count < len && !(input[count] < 0.0)){
		count++;
	}
	return count;
}


/* save the info about the percentage of wet, dry
   and utility areas to the array 'added' */
void
save_report_info(double* input, int input_len, double* added, int st){

	int count;
	double dry_area = 0.0;
	double wet_area = 0.0;
	double utility_area = 0.0;
	double total_area = 0.0;

	/* I only care about the numbers in third and fourth column, so count
	is incremented by four each time */
	for(count = 1; count < input_len; count += CYCLE){
		int room_type = *(input+count);
		double room_len = *(input+count+2);
		double room_width = *(input+count+3);
		double room_area = room_len*room_width;

		/* accumulate the area of dry room */
		dry_area += room_area*(room_type == HALLWAY ||
			room_type == BEDROOM || room_type == LIVING);

		/* accumulate the area of wet room */
		wet_area += room_area*(room_type == BATHROOM ||
			room_type == KITCHEN || room_type == LAUNDRY);

		/* accumulate the area of utility room */
		utility_area += room_area*(room_type == STORAGE ||
			room_type == GARAGE || room_type == BALCONY);
	}

	/* sum up all the area */
	total_area = dry_area + wet_area + utility_area;

	double dry_percent = (dry_area/total_area)*100;
	double wet_percent = (wet_area/total_area)*100;
	double utility_percent = (utility_area/total_area)*100;

	/* pass in all the useful summary information to the indicated
		array starting from a specific position st, so it always stack
		up in sequence. */
		added[st]   = *(input);
		added[st+1] = dry_area;
		added[st+2] = dry_percent;
		added[st+3] = wet_area;
		added[st+4] = wet_percent;
		added[st+5] = utility_area;
		added[st+6] = utility_percent;

	return;
}

/* format the summary report */
void
print_report(double* sub, int len){

	int count;

	printf("%s\n", DIV);
	printf("%s\n", HDR);
	printf("%s\n", DIV);

	for(count=0; count<len; count+=7){
		printf("|");
		printf("  %d  |", (int)(*(sub+count)));
		/* since the room area could only reach as large
		as 999.9 m^2, so a space of 8 characters would
		be enough */
		printf("%8.2lf", sub[count+1]);
		printf("%6.1lf%%  |", sub[count+2]);
		printf("%8.2lf", sub[count+3]);
		printf("%6.1lf%%  |", sub[count+4]);
		printf("%8.2lf", sub[count+5]);
		printf("%6.1lf%%  ", sub[count+6]);
		printf("|\n");
	}

	printf("%s\n", DIV);

	return;
}/* handle extreme cases */


/* copy all the numbers(int or double) in the text, and pass them to
   the array */
int
input_processing(double* input){

	double input_num = 0.0;
	int count = 0;

	while(scanf("%lf", &input_num) == 1){
		input[count++] = input_num;
	}

	return count;
}/* handle extreme cases */


/* to avoid making a single function over-complicated, this function
   is going identify the room types based on the number in the input
   text file. */
char*
roomtype_identifier(int room_type){

	if(room_type == HALLWAY){
		return "Hallway";
	}else if(room_type == UNUSED){
		return "Unused";
	}else if(room_type == BEDROOM){
		return "Bedroom";
	}else if(room_type == LIVING){
		return "Living";
	}else if(room_type == BATHROOM){
		return "Bathroom";
	}else if(room_type == KITCHEN){
		return "Kitchen";
	}else if(room_type == LAUNDRY){
		return "Laundry";
	}else if(room_type == STORAGE){
		return "Storage";
	}else if(room_type == GARAGE){
		return "Garage";
	}else if(room_type == BALCONY){
		return "Balcony";
	}else{
		return "Invalid";
	}
} /* End of Program */


/*========================Author's Note============================
   the comment 'handle extreme cases' ending after
   some indicated functions means, the function should
   be robust enough to run extreme cases as indicated
   by the assignment specification, where 100 apartments with 100
   rooms in each, with each room a sidelength of 99.9m and an
   area of 999.9m^2 may occur.
   These function should print out expected format for any input
   parameters within this limit.
  ================================================================*/
