#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE  
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>


// constants
#define		true			1
#define		false			0
#define		buffer			200
#define		structSize		50
#define		minTime			0
#define		maxTime			100
#define		scoreStatus		0
#define		scoreValue		1
#define		scoreRealVal	2
#define		lowPressure		2
#define		highPressure	3


// structures
struct BT {
	int time;
	float value;
};


struct BP {
	int time;
	int bpd;
	int bps;
};


struct HR {
	int time;
	float value;
};


// global vars
struct BT bt[structSize] = { 0 };
struct BP bp[structSize] = { 0 };
struct HR hr[structSize] = { 0 };
int btIndex = 0, bpIndex = 0, hrIndex = 0;


int isInt(char* str) {
	// -1 because of '\n'
	for (unsigned int i = 0; i < strlen(str) - 1; i++) {
		if (isdigit((int)(str[i])) == 0) {
			return false;
		}
	}
	return true;
}


int isFloat(char* str) {
	int hasFoundDecimal = false;

	for (unsigned int i = 0; i < strlen(str); i++) {
		if (isdigit((int)(str[i])) == 0) {
			if (str[i] == '.' && hasFoundDecimal == 0) {
				hasFoundDecimal = true;
			}
			else {
				return false;
			}
		}
	}
	return true;
}


int isIntInRange(int num, int lowerbound, int upperbound) {
	if (num >= lowerbound && num <= upperbound) {
		return true;
	}
	else {
		return false;
	}
}


int isValidMenuOption(char* menuOption) {
	if (strlen(menuOption) != 1) {
		return false;
	}
	else if (((menuOption[0]) >= 'a' && (menuOption[0]) <= 'e') ||
		((menuOption[0]) >= 'A' && (menuOption[0]) <= 'E')) {
		return true;
	}
	else {
		return false;
	}
}


void deallocate(char** strs, int size) {
	for (int i = 0; i < size; i++) {
		free(strs[i]);
	}
	free(strs);
}


char getMenu() {
	printf("%s\n", "a) Enter file location");
	printf("%s\n", "b) Time interval");
	printf("%s\n", "c) Get health score");
	printf("%s\n", "d) Get statistical values");
	printf("%s\n", "e) Exit");

	int isValidInput = false;
	char input[buffer];

	do {
		printf("%s: ", "Your choice");
		scanf("%[^\n]%*c", input);

		if (isValidMenuOption(input) == false) {
			printf("%s\n", "Invalid  choice dumbass. Enter again.");
		}

	} while (isValidMenuOption(input) == false);

	return input[0];
}


char* getFileLocation() {
	int isValidInput = false;
	char input[buffer];
	char* fullPath = NULL;
	FILE* file = NULL;

	do {
		printf("%s", "Enter file location (folder): ");
		fgets(input, buffer, stdin);
		size_t ln = strlen(input) - 1;
		if (input[ln] == '\n') {
			input[ln] = '\0';
		}

		fullPath = (char*)malloc(1 + strlen(input) + 9);
		strcpy(fullPath, input);
		strcat(fullPath, "\\BP_1.txt");

		file = fopen(fullPath, "r");

		if (!file) {
			printf("%s\n", "Incorrect path");
			isValidInput = false;
			memset(fullPath, 0, strlen(fullPath));
			memset(input, 0, strlen(input));
		}
		else {
			isValidInput = true;
			fclose(file);
			free(fullPath);
		}

	} while (!isValidInput);

	char* folder = (char*)malloc(strlen(input));
	strcpy(folder, input);
	return folder;
}


void readFiles(const char* folder) {
	char line[buffer] = { 0 };
	char* token = NULL;
	int length = strlen(folder) + 20;
	char* fileName = (char*)malloc(length);
	FILE* file = NULL;
	const char* DEL = "\t";

	// BP
	for (int i = 1; i <= 5; i++) {
		snprintf(fileName, length, "%s\\BP_%d.txt", folder, i);
		file = fopen(fileName, "r");

		while (fgets(line, buffer, file) != NULL) {
			token = strtok(line, DEL);
			bp[bpIndex].time = atoi(token);
			token = strtok(NULL, DEL);
			bp[bpIndex].bpd = atoi(token);
			token = strtok(NULL, DEL);
			bp[bpIndex++].bps = atoi(token);
		}

		fclose(file);
		memset(fileName, 0, length);
	}

	// BT
	for (int i = 1; i <= 5; i++) {
		snprintf(fileName, length, "%s\\BT_%d.txt", folder, i);
		file = fopen(fileName, "r");

		while (fgets(line, buffer, file) != NULL) {
			token = strtok(line, DEL);
			bt[btIndex].time = atoi(token);
			token = strtok(NULL, DEL);
			bt[btIndex++].value = (float) atof(token);
		}

		fclose(file);
		memset(fileName, 0, length);
	}

	// HR
	for (int i = 1; i <= 5; i++) {
		snprintf(fileName, length, "%s\\HR_%d.txt", folder, i);
		file = fopen(fileName, "r");

		while (fgets(line, buffer, file) != NULL) {
			token = strtok(line, DEL);
			hr[hrIndex].time = atoi(token);
			token = strtok(NULL, DEL);
			hr[hrIndex++].value = (float) atof(token);
		}

		fclose(file);
		memset(fileName, 0, length);
	}

	free(fileName);
	printf("%s\n", "Finish populating data");
}


int getTime(char* whatTime, int min) {
	int time = -1;
	char input[buffer] = { 0 };

	do {
		printf("Enter %stime: ", whatTime);
		fgets(input, buffer, stdin);

		if (!isInt(input)) {
			printf("Error: not an integer\n");
			continue;
		}

		time = atoi(input);
		if (!isIntInRange(time, min, maxTime)) {
			printf("Error: time must be between %d and %d\n", min, maxTime);
			time = -1;
			continue;
		}

	} while (time == -1);

	return time;
}


char** getTemp(int time) {
	float totalTemp = 0;
	int totalFound = 0;
	char ** results = (char**)malloc(3);

	for (int i = 0; i < btIndex; i++) {
		if (bt[i].time == time) {
			totalTemp += bt[i].value;
			totalFound++;
		}
	}

	if (totalFound == 0) {
		free(results);
		return NULL;
	}

	float average = totalTemp / totalFound;

	if (97 <= average && average <= 99) {
		results[scoreStatus] = (char*)malloc(8);
		results[scoreStatus] = "Normal";
		results[scoreValue] = (char*)malloc(2);
		results[scoreValue] = "1";
	}
	else {
		results[scoreStatus] = (char*)malloc(10);
		results[scoreStatus] = "Abnormal";
		results[scoreValue] = (char*)malloc(2);
		results[scoreValue] = "0";
	}
	results[scoreRealVal] = (char*)malloc(10);
	snprintf(results[scoreRealVal], 10, "%f", average);

	return results;
}

// working on this
char** getPress(int time) {
	int totalFound = 0, lowPress = INT_MAX, highPress = INT_MIN;
	float totalBpd = 0, totalBps = 0, averageBpd = 0, averageBps = 0;
	char **results = (char**)malloc(4);
	char* status = NULL;

	for (int i = 0; i < btIndex; i++) {
		if (bp[i].time == time) {
			totalBpd += bp[i].bpd;
			totalBps += bp[i].bps;
			totalFound++;
		}
	}

	if (totalFound == 0) {
		free(results);
		return NULL;
	}
	else {
		averageBpd = totalBpd / totalFound;
		averageBps = totalBps / totalFound;
	}

	if (averageBpd <= 80 && averageBps <= 120) {
		results[scoreStatus] = (char*)malloc(10);
		results[scoreStatus] = "Normal";
		results[scoreValue] = (char*)malloc(2);
		results[scoreValue] = "1";
	}
	else if ((80 < averageBpd && averageBpd <= 89) ||
		(120 < averageBps && averageBps <= 139)) {
		results[scoreStatus] = (char*)malloc(16);
		results[scoreStatus] = "Prehypertension";
		results[scoreValue] = (char*)malloc(4);
		results[scoreValue] = "0.5";
	}
	else if (averageBpd >= 90 || averageBps >= 140) {
		results[scoreStatus] = (char*)malloc(16);
		results[scoreStatus] = "Hypertension";
		results[scoreValue] = (char*)malloc(2);
		results[scoreValue] = "0";
	}
	/*else {	// never happen
		free(results);
		return NULL;
	}*/

	results[lowPressure] = (char*)malloc(10);
	snprintf(results[lowPressure], 10, "%d", (int)averageBpd);
	results[highPressure] = (char*)malloc(10);
	snprintf(results[highPressure], 10, "%d", (int)averageBps);

	return results;
}


char** getRate(int time) {
	float totalRate = 0;
	int totalFound = 0;
	char ** results = (char**)malloc(3);

	for (int i = 0; i < btIndex; i++) {
		if (hr[i].time == time) {
			totalRate += hr[i].value;
			totalFound++;
		}
	}

	if (totalFound == 0) {
		free(results);
		return NULL;
	}

	float average = totalRate / totalFound;

	if (60 <= average && average <= 100) {
		results[scoreStatus] = (char*)malloc(7);
		results[scoreStatus] = "Normal";
		results[scoreValue] = (char*)malloc(2);
		results[scoreValue] = "1";
	}
	else {
		results[scoreStatus] = (char*)malloc(13);
		if (average > 100) {
			results[scoreStatus] = "Tachycardia";
		}
		else {
			results[scoreStatus] = "Bradycardia";
		}
		results[scoreValue] = (char*)malloc(2);
		results[scoreValue] = "0";
	}
	results[scoreRealVal] = (char*)malloc(10);
	snprintf(results[scoreRealVal], 10, "%f", average);

	return results;
}


void getHealthScore(int time) {
	// calculate
	char** temp = getTemp(time);
	char** rate = getRate(time);
	char** press = getPress(time);	// correct

	if (temp == NULL || press == NULL || rate == NULL) {
		printf("Error: cannot find data for time %d\n", time);
		return;
	}

	double score = 20 * atof(temp[scoreValue]) + 25 * atof(press[scoreValue]) + 30 * atof(rate[scoreValue]) + 25;

	// display
	printf("%s\n", "Body Temperature     | Blood Pressure         | Heart Rate");
	printf("%s\n", "-------------------- + ---------------------- + ------------------");
	printf("%-4.1fF%16c| %-3d - %-3d mm Hg        | %d bpm\n", atof(temp[scoreRealVal]), ' ', atoi(press[lowPressure]), atoi(press[highPressure]), (int) atof(rate[scoreRealVal]));
	printf("%-21s| %-23s| %-23s\n", temp[scoreStatus], press[scoreStatus], rate[scoreStatus]);
	printf("%s\n", "-------------------- + ---------------------- + ------------------");
	printf("At time %d, the Health Score was %d/100\n", time, (int) score);
	printf("%s\n", "------------------------------------------------------------------");

	/*deallocate(temp, 2);
	deallocate(press, 2);
	deallocate(rate, 2);*/
}


void getStatValues(int startTime, int endTime) {
	// calculate 

	// display
	printf("%s\n", "--------------------------------------------------------");
	printf("      Statistical Values between time %d and %d\n", startTime, endTime);
	printf("%s\n", "---------+-----------+---------------------+------------");
	printf("%s\n", "Variable + Body Temp + Blood Pressure      + Heart Rate ");
	printf("%s\n", "---------+-----------+---------------------+------------");
	printf("%8s |%6.2fF%4c|%6.1f -%6.1f mm Hg | %5.1f bpm\n", "Min", 97.00, ' ', 76.0, 118.0, 62.0);
	printf("%8s |%6.2fF%4c|%6.1f -%6.1f mm Hg | %5.1f bpm\n", "Max", 98.00, ' ', 79.0, 119.0, 68.0);
	printf("%8s |%6.2fF%4c|%6.1f -%6.1f mm Hg | %5.1f bpm\n", "Average", 97.50, ' ', 77.0, 118.0, 65.3);
	printf("%8s |%6.2fF%4c|%6.1f -%6.1f mm Hg | %5.1f bpm\n", "StdDev", 0.50, ' ', 1.5, 0.5, 3.0);
	printf("%s\n", "---------+-----------+---------------------+------------");
}


int main(int argc, const char** argv) {
	char menuOption = 0;
	char* fileLocation = NULL;
	int hasFileLocation = false;
	int hasTimes = false;
	int startTime = 0, endTime = 0, time = 0;

	do {
		menuOption = getMenu();
		printf("\n---------------------------\n\n");

		switch (menuOption) {
			case 'a':
			case 'A':
				fileLocation = getFileLocation();
				hasFileLocation = true;					
				readFiles(fileLocation);
				break;
			case 'b':
			case 'B':
				startTime = getTime("start ", minTime);
				endTime = getTime("end ", startTime);
				hasTimes = true;
				printf("%s\n", "Complete updating time frame");
				break;
			case 'c':
			case 'C':
				if (!hasFileLocation) {
					printf("%s\n", "Error: no data");
				}
				else {
					time = getTime("", minTime);
					getHealthScore(time);
				}
				break;
			case 'd':
			case 'D':
				if (!hasFileLocation) {
					printf("%s\n", "Error: no data");
				}
				else if (!hasTimes) {
					printf("%s\n", "Error: no time frame");
				}
				else {
					getStatValues(startTime, endTime);
				}
				break;
				break;
			case 'e':
			case 'E':
				printf("%s\n", "Bye!");
				break;
			default:
				break;
		}

		printf("\n---------------------------\n\n");

	} while (menuOption != 'e' && menuOption != 'E');
}
