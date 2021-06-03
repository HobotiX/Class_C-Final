/***********************************************************************************
	Name: Daniel Adams
	Class: CS133C, Winter 2018
	Date: March 3rd, 2018
	Project #: Final Project
	Driver Name: vanscheduler.c
	Program Description: This program will read two files with companies and products,
		Display them to the user, then allow the user to create a custom order sheet.
	Test Oracle: FinalPROJECT-TestOracle.pdf
************************************************************************************/
/***********************************************************************************
	A company needs a computer program written in the C Programming Language that
	will provide a transport scheduling system:
		a.) Read an input file of the available stops. Each stop will have the
		following delimited information:
			1.) Id
			2.) BuildingName
		b.) Read an input file of the available vans. Each van should have the
		following delimited information:
			1.) Id
			2.) AvailableStartTime
			3.) Available Stop Time
			4.) Lunch Hour (assume 1 hour for lunch)
			5.) Building IDs where the van stops
				Example: for a van 1 that starts at 8, takes lunch at 12, and ends at 5
					and stops at location 1,3,5,7:
					1,8,5,12,1,3,5,7 
		c.) Accept user input to schedule a transport. Users will request:
			1.) Destination
			2.) Date
			3.) Start Time
			4.) Total Time
		d.) Build a schedule from user input. Determine if a van is available, if there
		is a van available add an entry in the schedule and let the user know their
		transport is scheduled. If no van is available let the user know you are unable
		to meet their request.
		e.) Save the schedule to an output file each time the schedule is updated.
		f.) Allow the user a choice to display the entire schedule for each van, by day
		and hour. For example:
			Van Id: 1
			Date: 2/22/2018
			8:00 Hospital 9:00 Open 10:00 Clinic 11:00Hospital
			12:00 Lunch
		g.) Continue until user requests an exit.
************************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define MAXVAN 8
#define MAXSTOPS 21

	// Defining Global Variables that will be used throughout the application
char
	vansVanID[MAXVAN][3],			// Van ID's from vans file is stored in this array.
	vansStopID[MAXVAN][101],		// Stop ID's from vans file is stored in this array.
	stopsStopID[MAXSTOPS][5],		// Stop ID's from stops file is stored in this array.
	stopsStopName[MAXSTOPS][51];	// Stop Name's from stops file are stored in this array.
int
	vansStartTime[MAXVAN],			// schedule Start Times from vans file is stored in this array.
	vansEndTime[MAXVAN],			// schedule End Times from vans file is stored in this array.
	vansLunchTime[MAXVAN],			// scheduled Lunch Times from vans file is store in this array.
	databaserArr[MAXVAN][MAXSTOPS],	// Database 2d array that connexts the vans and its stop locations.
	availableHours[25],				// Hours when a particular van is available (1 = available, 0 = unavailable)
	stopChoice,						// This is the specific stop the user wants to go to (int 1-20)
	timeAMPM,						// This indicates what time of day (1 = AM, 2 = PM)
	pickUpTime,						// This is the time that the user wishes to be picked up (1-12)
	vanNumberPick,					// This is the number of the van which the user wishes to use.
	vanStopTime[MAXVAN],			// This indicates which van goes to the indicated stop (1 = yes, 0 = no)
	scheduleNotOrdered = 0,			// This checks if the user has gone through this method before or not.
	scheduledDay,					// This is the scheduled day
	scheduledMonth,					// This is the scheduled month
	scheduledYear,					// This is the scheduled year
	scheduledTime[MAXVAN][25],		// This is the shcedualed times for a specific van
	scheduledLocation[MAXVAN][25][MAXSTOPS], // This is the location that the van will be going to.
	vanArrRegistry;	
bool
	continueYN = false,
	timeCheckConfirmed = false;

/***********************************************************************************
	Database Builder:
		This purpose of this method is to create a database method made up of single
		integers that will reference a specific index in the stop locations. These
		integers are tied directly to the vans include that stop in their rotation.
************************************************************************************/
void stopsDatabaser() {
		// Declaring variables that will live and die within this method.
	int
		dataChecker = 0,
		databasePositionAdjustment = 0;	
	char
		stopChecker[5];
	
		//Starting with a for loop that will step through each van.
	for (int i = 0; i < MAXVAN; i++) {
			// Starting a second for loop that will gather the stop ID data, making sure there are enough repeats to capture all of the data.
			//printf("Van#: %d ",i+1);
		for (int k = 0; k < 101; k++) {
			//*******************
				// Looks for an value that is not an underscore and captures the data.
			strncpy(stopChecker, &vansStopID[i][k],4);
			if (strlen(stopChecker) != 0) {
				dataChecker = 0; // sets (and resets) to zero.
				// Tallies up the datachecker looking for a match in the other array, then bounces when found.
				//printf("Check: %s ",stopChecker);
				int matchConfirmation = 1;
				do {
					//stopIDInt = stopsStopID[dataChecker] - '0';
					matchConfirmation = strcmp(stopChecker, stopsStopID[dataChecker]);
					//printf("To: %s ", stopsStopID[dataChecker]);
					dataChecker = dataChecker + 1;	// adds one in each pass.
				} while (matchConfirmation != 0);
				//printf(" (%d) ", dataChecker);
					// Adjusts the position of k for the databaser array
				if (k != 0) {
					databasePositionAdjustment = k / 5;
				}
				databaserArr[i][databasePositionAdjustment] = dataChecker; // adds number to the databaserArr
				k = k + 4;
				databasePositionAdjustment = 0;
			}
		}
	}
}
/***********************************************************************************
	File Readers: Van schedule Reader
		This method reads the information stored in the van schedule file and stores
		that information into 2D arrays.
************************************************************************************/
void readVanFile(FILE *vanf) {
	int
		i = 0;
		
	while (fscanf(vanf, "%s %d %d %d %s", vansVanID[i], &vansStartTime[i], &vansEndTime[i], &vansLunchTime[i], vansStopID[i])!= EOF) {
		//printf("%s %d %d %d %s", (vArr + i)->vanID, &(vArr + i)->startTime, &(vArr + i)->endTime, &(vArr + i)->lunchTime, (vArr + i)->stopID);
		i++;
	}
}
/***********************************************************************************
	File Readers: Stop Information Reader
		This method reads the information stored in the stop information file and stores
		that information into 2D arrays.
************************************************************************************/
void readStopFile(FILE *stopf) {
	int
		i = 0;
		
	while (fscanf(stopf, "%s %s", stopsStopID[i], stopsStopName[i])!= EOF) {
		for (int j = 0; j <51; j++) {
			if (stopsStopName[i][j] == '_') {
				stopsStopName[i][j] = ' ';
			}
		}
		i++;
	}
}

/***********************************************************************************
	Look at Available Times:
		This method finds the vans that make stop at the previously identified
		location, then prints the vans schedule so that the user knows when the van
		is, and is not available. 
************************************************************************************/
void getAvailableTimes() {
		// use this to figure out which vans stop at that location using 1 for yes, or 0 for no.
	// Reset the vanStops
	for (int l = 0; l < MAXVAN - 1;++l) {
		vanStopTime[l] = 0;
	}
	for (int i = 0; i < MAXVAN - 1; ++i) {
		//printf("VAN#:%d -> ", i+1);
		for (int j = 0; j < MAXSTOPS - 1; ++j) {
			if (databaserArr[i][j] != 0) {
				//printf("%d == %d ",databaserArr[i][j], stopChoice);
				if (databaserArr[i][j] == stopChoice) {
					vanStopTime[i] = 1;
					//printf("(%d) / ", vanStopTime[i]);
				}
			}
		}
	}
	printf("\n\n--- Vans that stop at that location ---");
		// Sends out that information, converting military time to am and pm time. Will only print the schedules of the relevent vans as indicated by vanStopTimes array.
	for (int k = 0; k < MAXVAN - 1; k++) {
		if (vanStopTime[k] == 1) {
			printf("\nVan #%d.)\n\tStart Time: ", k + 1);
				// start times *****************************
			if (vansStartTime[k] == 12) {
				printf("12:00 PM");
			}
			else if (vansStartTime[k] > 12) {
				printf("%d:00 PM", vansStartTime[k]-12);
			}
			else if (vansStartTime[k] == 0) {
				printf("12:00 AM");
			}
			else {
				printf("%d:00 AM", vansStartTime[k]);
			}
			
				// lunch times ****************************
			printf("\n\tUnavailable from: ");
			if (vansLunchTime[k] == 12) {
				printf("12:00 PM");
			}
			else if (vansLunchTime[k] > 12) {
				printf("%d:00 PM", vansLunchTime[k]-12);
			}
			else if (vansLunchTime[k] == 0) {
				printf("12:00 AM");
			}
			else {
				printf("%d:00 AM", vansLunchTime[k]);
			}
			
				// lunch time ends *************************
			printf(" to ");
			int
				lunchEnd = vansLunchTime[k] + 1;
			if (lunchEnd == 12) {
				printf("12:00 PM");
			}
			else if (lunchEnd > 12) {
				printf("%d:00 PM", lunchEnd-12);
			}
			else if (lunchEnd == 0) {
				printf("12:00 AM");
			}
			else {
				printf("%d:00 AM", lunchEnd);
			}
			
				// end times ********************************
			printf("\n\tEnds service at: ");
			if (vansEndTime[k] == 12) {
				printf("12:00 PM");
			}
			else if (vansEndTime[k] > 12) {
				printf("%d:00 PM", vansEndTime[k]-12);
			}
			else if (vansEndTime[k] == 0) {
				printf("12:00 AM");
			}
			else {
				printf("%d:00 AM", vansEndTime[k]);
			}
		}
	}
}
/***********************************************************************************
	File Readers: Van schedule Reader
		This method reads the information stored in the van schedule file and stores
		that information into 2D arrays.
************************************************************************************/
void timeChecker() {
	if (vansStartTime[vanNumberPick - 1] <= pickUpTime && vansLunchTime[vanNumberPick - 1] > pickUpTime) {
		timeCheckConfirmed = true;
		scheduledTime[vanArrRegistry][pickUpTime - 1] = 1;
		scheduledLocation[vanArrRegistry][pickUpTime - 1][stopChoice] = 1;
		scheduleNotOrdered = 1;
	}
	else if (vansLunchTime[vanNumberPick - 1] + 1 <= pickUpTime && vansEndTime[vanNumberPick - 1] > pickUpTime) {
		timeCheckConfirmed = true;
		scheduledTime[vanArrRegistry][pickUpTime - 1] = 1;
		scheduledLocation[vanArrRegistry][pickUpTime - 1][stopChoice] = 1;
		scheduleNotOrdered = 1;
	}
	else {
		timeCheckConfirmed = false;
		printf("\nThat is not a valid time.\nPlease enter an hour that van is available.");
	}
}

/***********************************************************************************
	schedule a Date for Pickup:
		This method will figure out what day you would like to schedule the van for.
		This date information is then stored in a global variable that can then be
		accessed when the program is ready to print that information.
************************************************************************************/
void schadualDay() {
	// Retreaving the current date from the system.
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	// declaring variables that will be used throughout this method.
	int
		confirmationYN,
		currentDay = tm.tm_mday,
		currentMonth = tm.tm_mon + 1,
		currentYear = tm.tm_year + 1900,
		numberOfDays = 0,
		scheduledDayAug = 0; 
		
	// begin a do/while loop that confirms the date indicated by the user.
	do {	
		// ***********************************
		// Find how many days in the future the user wants to ride.
		do {
				// Indicates to the user what information is being requested.
				printf("\n\nTodays date is %d/%d/%d", currentMonth, currentDay, currentYear);
				printf("\nIn how many days would you like to schedule a van pickup?");
				printf("\n0 for today, 7 for one week from today, 30 for one month from today.");
				printf("\nYour choice (0-30): ");
				scanf("%d",&scheduledDayAug);
			} while (scheduledDayAug < 0 || scheduledDayAug >= 30);
		// ***********************************
		// Finds out how many days are in the month and accounts for leep years.
		if (currentMonth == 4 || currentMonth == 6 || currentMonth == 9 || currentMonth == 11){
			numberOfDays = 30;
		}
		else if (currentMonth == 2) {
			if (((currentYear % 4) == 0 && (currentYear % 100) != 0) || (currentYear % 400 == 0)) {
				numberOfDays = 29;
			}  
			else {
				numberOfDays = 28;  
			}
		}  
		else {
			numberOfDays = 31; 
		}
		// ***************************************
		// Finds what that future date is.
		if ((currentDay + scheduledDayAug) > numberOfDays) {
			scheduledDay = (currentDay + scheduledDayAug) - numberOfDays;
			if (currentMonth == 12) {
				scheduledMonth = 1;
				scheduledYear = currentYear + 1;
			}
			else {
				scheduledMonth = currentMonth + 1;
				scheduledYear = currentYear;
			}
		}
		else {
			scheduledDay = currentDay + scheduledDayAug;
			scheduledMonth = currentMonth;
			scheduledYear = currentYear;
		}
		// prints the future day that you would like to schedule transport.
		printf("\nYour van is scheduled for %d/%d/%d. Is this correct?", scheduledMonth, scheduledDay, scheduledYear);
		printf("\nIs this day correct\n1.) YES\n0.) NO");
		do {
			printf("\nYour choice (1 or 0): ");
			scanf("%d",&confirmationYN);
		} while (confirmationYN < 0 && confirmationYN > 1);
	} while (confirmationYN != 1);
}

/***********************************************************************************
	Available Stops Printer:
		This method will simply print the list of stops for the user with a number
		that is associated to it.
************************************************************************************/
void printschedule() {
	printf("\n--- List of van stops ---");
	for (int i = 0; i < MAXSTOPS - 1; ++i) {
		printf("\n%d.) %s", i + 1, stopsStopName[i]);
	}
}

/***********************************************************************************
	Stops Decision Tree:
		This method is used to find which stop the used would like to go to.
************************************************************************************/
void stopChoiceTree() {
	printf("\n-------------------------");
	printf("\nWhat stop would you like to go to?");
	do {
		printf("\nYour choice (1 - %d): ", MAXSTOPS - 1);
		scanf("%d",&stopChoice);
	} while (stopChoice < 1 && stopChoice > MAXSTOPS - 1);
}

/***********************************************************************************
	Continue Tree:
		This method is used to determine if the user would like to schedule
		something or stop.
************************************************************************************/
void userChoiceTree() {
	int
		ioContinue;
	printf("\nWould you like to schedule a stop?");
	printf("\n1.) YES\n0.) NO");
	do {
		printf("\nYour choice (0 or 1): ");
		scanf("%d",&ioContinue);
	} while (ioContinue != 1 && ioContinue != 0);
	switch (ioContinue) {
		case 1:
			continueYN = false;
			break;
		case 0:
			continueYN = true;
			break;
	}
}

/***********************************************************************************
	Time Choice Tree:
		This method is used to find what time the user would like to schedule a
		pickup for on the previously indicated day.
************************************************************************************/
void timeChoiceTree() {
	// add van choice as well!!!
	int
		howManyVans = 0,
		lastVan = 0;
	vanArrRegistry = 0;
	printf("\n\n-------------------------");
	for (int i = 0; i < MAXVAN; i++) {
		if (vanStopTime[i] == 1) {
			howManyVans++;
			lastVan = i;
		}
	}
	if (howManyVans > 1) {
		printf("\nWhat van would you like to schedule?");
		printf("\nThe vans available are:\n");
		for (int j = 0; j < MAXVAN; j++) {
			if (lastVan == j) {
				printf("and %d", j + 1);
			}
			else if (vanStopTime[j] == 1) {
				printf("%d, ", j + 1);
			}
		}
		do {
			printf("\nYour choice: ");
			scanf("%d",&vanNumberPick);
		} while (vanNumberPick < 1 && vanNumberPick > (MAXVAN - 1));
	}
	
	
	printf("\nWhat time would you like to schedule a pickup?");
	do {
		printf("\nYour choice (1 - 12): ");
		scanf("%d",&pickUpTime);
	} while (pickUpTime < 1 && pickUpTime > 12);
	printf("\nIs that AM or PM?");
	printf("\n1.) AM\n0.) PM");
	do {
		printf("\nYour choice (1 or 0): ");
		scanf("%d",&timeAMPM);
	} while (timeAMPM != 1 && timeAMPM != 0);
	vanArrRegistry = vanNumberPick - 1;
	//scheduledTime[vanArrRegistry] = 1;
	if (timeAMPM == 0) {
		if (pickUpTime < 12) {
			pickUpTime = pickUpTime + 12;
		}
	}
	else if (timeAMPM == 1) {
		if (pickUpTime == 12) {
			pickUpTime = 0;
		}
	}
	
	printf("\n----------------------------");
}

/***********************************************************************************
	Exit Confirmation Tree:
		This method is only used to confirm that the user does indeed wish to exit
		the application.
************************************************************************************/
void exitConfirmation() {
	int
		ioContinue;
	printf("\nAre you sure you would like to exit the program?");
	printf("\n1.) YES\n0.) NO");
	do {
		printf("\nYour choice (0 or 1): ");
		scanf("%d",&ioContinue);
	} while (ioContinue != 1 && ioContinue != 0);
	switch (ioContinue) {
		case 1:
			continueYN = true;
			break;
		case 0:
			continueYN = false;
			break;
	}
}

/***********************************************************************************
	schedule Printer:
		This method is used to print the schedule to the out file.
************************************************************************************/
void schedulePrinter(FILE *schedulef) {
	int vanDatePrinter;
	if (scheduleNotOrdered != 0) {
		// i = Van number
		// j = time
		// k = stop location
		for (int i = 0; i < MAXVAN - 1; i++) {
			vanDatePrinter = 0;
			for (int j = 0; j < 24; j++) {
				if (scheduledTime[i][j] == 1) {
					if (vanDatePrinter == 0) {
						fprintf(schedulef, "Van ID: %d \nDate: %d/%d/%d\n", i + 1, scheduledMonth, scheduledDay, scheduledYear);
						vanDatePrinter = 1;
					}
					for (int k = 0; k < MAXSTOPS - 1; k++) {
						if (scheduledLocation[i][j][k] == 1) {
							if (j == 23) {
								fprintf(schedulef, "- %d:00 AM\t%s\n", j - 11, stopsStopName[k-1]);
							}
							else if (j > 11) {
								fprintf(schedulef, "- %d:00 PM\t%s\n", j - 11, stopsStopName[k-1]);
							}
							else if (j == 11) {
								fprintf(schedulef, "- %d:00 PM\t%s\n", j + 1, stopsStopName[k-1]);
							}
							else {
								fprintf(schedulef, "- %d:00 AM\t%s\n", j + 1, stopsStopName[k-1]);
							}
						}
					}
				}
			}
		}
	}
	else {
		printf("\nSorry, that time slot is not available.");
		printf("\nPlease try again.");
	}
}

/***********************************************************************************
	Introduction Text:
		This text is displayed when the application first opens up.
************************************************************************************/
void programIntroduction() {
	printf("\n*** VAN scheduleR ***");
	printf("\n\nThis application allows you, the user to schedule where you would like to go");
	printf("\nAnd the locations that are available at that time.");
}

/***********************************************************************************
	Closing Text:
		This method is only called right before the application closes after the user
		confirms that they want to close the application.
************************************************************************************/
void programClosing() {
	printf("\nThank you for using this program.");
	printf("\nYour schedule can be was exported to routeOrder.txt");
	printf("\nGoodbye.");
}

/***********************************************************************************
	MAIN:
		This is the main method that is used to call all of the other methods. This 
		is where the files are opened, and where the main program loop is located.
************************************************************************************/
int main(int argc, char *argv[]) {
	//*******************************************************************************
	char
		fnAvailableVans[] = "vanAvailable.txt",
		fnStopsAvailable[] = "vanStops.txt",
		fnRoutesOrdered[] = "routeOrder.txt";
		
	FILE
		*fwOrder = fopen(fnRoutesOrdered, "w"),
		*frVans = fopen(fnAvailableVans, "r"),
		*frStops = fopen(fnStopsAvailable, "r");
	//*******************************************************************************
	// Prepare arrays with needed information.
	readVanFile(frVans);
	readStopFile(frStops);
	stopsDatabaser();
	
	// Start User Side Program
	programIntroduction();
	schadualDay();
	
	// Start Primary Loop Question Loop
	while (continueYN != true) {
		userChoiceTree();
		// If User wants to add a pickup
		if (continueYN != true) {
			printschedule();
			stopChoiceTree();
			getAvailableTimes();
			do {
				timeChoiceTree();
				timeChecker();
			} while (timeCheckConfirmed != true);
			
			//schedulePrinter(fwOrder);
		}
		// Makes sure the user wants to stop the application.
		else {
			exitConfirmation();
		}
	}
	// Indicates that the program is indeed closing
	schedulePrinter(fwOrder);
	programClosing();
	
	// Closes all files and ends the program
	fclose(fwOrder);
	fclose(frVans);
	fclose(frStops);
	return (0);
}