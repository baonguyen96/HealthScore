#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE  
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<math.h>
#include<limits.h>
#include<float.h>


// constants
#define		true					1
#define		false					0
#define		BUFFER					200
#define		STRUCT_SIZE				50
#define		MIN_TIME				0
#define		MAX_TIME				100
#define		SCORE_STATUS			0
#define		SCORE_VALUE				1
#define		SCORE_REAL_VALUE		2
#define		LOW_PRESSURE			2
#define		HIGH_PRESSURE			3
#define		STAT_MIN				0
#define		STAT_MAX				1
#define		STAT_AVERAGE			2
#define		STAT_DEVIATION			3
#define		STAT_MIN_BPD			0
#define		STAT_MAX_BPD			1
#define		STAT_AVERAGE_BPD		2
#define		STAT_DEVIATION_BPD		3
#define		STAT_MIN_BPS			4
#define		STAT_MAX_BPS			5
#define		STAT_AVERAGE_BPS		6
#define		STAT_DEVIATION_BPS		7


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
struct BT bt[STRUCT_SIZE] = { 0 };
struct BP bp[STRUCT_SIZE] = { 0 };
struct HR hr[STRUCT_SIZE] = { 0 };
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
	printf("%s:\n", "Menu");
	printf("   %s\n", "a) Enter file location");
	printf("   %s\n", "b) Time interval");
	printf("   %s\n", "c) Get health score");
	printf("   %s\n", "d) Get statistical values");
	printf("   %s\n", "e) Exit");

	int isValidInput = false;
	char input[BUFFER];

	do {
		printf("%s: ", "Your choice");
		scanf("%[^\n]%*c", input);

		if (isValidMenuOption(input) == false) {
			printf("%s\n", "Invalid  choice. Enter again.");
		}

	} while (isValidMenuOption(input) == false);

	return input[0];
}


char* getFileLocation() {
	int isValidInput = false;
	char input[BUFFER];
	char* fullPath = NULL;
	FILE* file = NULL;

	do {
		printf("%s", "Enter file location (folder): ");
		fgets(input, BUFFER, stdin);
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
	char line[BUFFER] = { 0 };
	char* token = NULL;
	int length = strlen(folder) + 20;
	char* fileName = (char*)malloc(length);
	FILE* file = NULL;
	const char* DEL = "\t";

	// BP
	for (int i = 1; i <= 5; i++) {
		snprintf(fileName, length, "%s\\BP_%d.txt", folder, i);
		file = fopen(fileName, "r");

		while (fgets(line, BUFFER, file) != NULL) {
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

		while (fgets(line, BUFFER, file) != NULL) {
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

		while (fgets(line, BUFFER, file) != NULL) {
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
	char input[BUFFER] = { 0 };

	do {
		printf("Enter %stime: ", whatTime);
		fgets(input, BUFFER, stdin);

		if (!isInt(input)) {
			printf("Error: not an integer\n");
			continue;
		}

		time = atoi(input);
		if (!isIntInRange(time, min, MAX_TIME)) {
			printf("Error: time must be between %d and %d\n", min, MAX_TIME);
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
		results[SCORE_STATUS] = (char*)malloc(8);
		results[SCORE_STATUS] = "Normal";
		results[SCORE_VALUE] = (char*)malloc(2);
		results[SCORE_VALUE] = "1";
	}
	else {
		results[SCORE_STATUS] = (char*)malloc(10);
		results[SCORE_STATUS] = "Abnormal";
		results[SCORE_VALUE] = (char*)malloc(2);
		results[SCORE_VALUE] = "0";
	}
	results[SCORE_REAL_VALUE] = (char*)malloc(10);
	snprintf(results[SCORE_REAL_VALUE], 10, "%f", average);

	return results;
}


char** getPress(int time) {
	int totalFound = 0;
	float totalBpd = 0, totalBps = 0, averageBpd = 0, averageBps = 0;
	char **results = (char**)malloc(4);

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
		results[SCORE_STATUS] = (char*)malloc(10);
		results[SCORE_STATUS] = "Normal";
		results[SCORE_VALUE] = (char*)malloc(2);
		results[SCORE_VALUE] = "1";
	}
	else if ((80 < averageBpd && averageBpd <= 89) ||
		(120 < averageBps && averageBps <= 139)) {
		results[SCORE_STATUS] = (char*)malloc(16);
		results[SCORE_STATUS] = "Prehypertension";
		results[SCORE_VALUE] = (char*)malloc(4);
		results[SCORE_VALUE] = "0.5";
	}
	else if (averageBpd >= 90 || averageBps >= 140) {
		results[SCORE_STATUS] = (char*)malloc(16);
		results[SCORE_STATUS] = "Hypertension";
		results[SCORE_VALUE] = (char*)malloc(2);
		results[SCORE_VALUE] = "0";
	}
	/*else {	// never happen
		free(results);
		return NULL;
	}*/

	results[LOW_PRESSURE] = (char*)malloc(10);
	snprintf(results[LOW_PRESSURE], 10, "%d", (int)averageBpd);
	results[HIGH_PRESSURE] = (char*)malloc(10);
	snprintf(results[HIGH_PRESSURE], 10, "%d", (int)averageBps);

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
		results[SCORE_STATUS] = (char*)malloc(7);
		results[SCORE_STATUS] = "Normal";
		results[SCORE_VALUE] = (char*)malloc(2);
		results[SCORE_VALUE] = "1";
	}
	else {
		results[SCORE_STATUS] = (char*)malloc(13);
		if (average > 100) {
			results[SCORE_STATUS] = "Tachycardia";
		}
		else {
			results[SCORE_STATUS] = "Bradycardia";
		}
		results[SCORE_VALUE] = (char*)malloc(2);
		results[SCORE_VALUE] = "0";
	}
	results[SCORE_REAL_VALUE] = (char*)malloc(10);
	snprintf(results[SCORE_REAL_VALUE], 10, "%f", average);

	return results;
}


void getHealthScore(int time) {
	// calculate
	char** temp = getTemp(time);
	char** rate = getRate(time);
	char** press = getPress(time);	

	if (temp == NULL || press == NULL || rate == NULL) {
		printf("Error: cannot find data for time %d\n", time);
		return;
	}

	double score = 20 * atof(temp[SCORE_VALUE]) + 25 * atof(press[SCORE_VALUE]) + 30 * atof(rate[SCORE_VALUE]) + 25;

	// display
	printf("%s\n", "------------------------------------------------------------------");
	printf("%s\n", "Body Temperature     | Blood Pressure         | Heart Rate");
	printf("%s\n", "-------------------- + ---------------------- + ------------------");
	printf("%-4.1fF%16c| %-3d - %-3d mm Hg        | %d bpm\n", 
		atof(temp[SCORE_REAL_VALUE]), ' ', atoi(press[LOW_PRESSURE]), 
		atoi(press[HIGH_PRESSURE]), (int) atof(rate[SCORE_REAL_VALUE]));
	printf("%-21s| %-23s| %-23s\n", temp[SCORE_STATUS], press[SCORE_STATUS], rate[SCORE_STATUS]);
	printf("%s\n", "-------------------- + ---------------------- + ------------------");
	printf("At time %d, the Health Score was %d/100\n", time, (int) score);
	printf("%s\n", "------------------------------------------------------------------");

	/*deallocate(temp, 2);
	deallocate(press, 2);
	deallocate(rate, 2);*/
}


float getStdDev(float* nums, int size, float average) {
	double sum = 0, var = 0;
	
	for (int i = 0; i < size; i++) {
		sum += pow((nums[i] - average), 2);
	}

	var = sum / size;

	return (float)sqrt(var);
}


char** getTempStats(int startTime, int endTime) {
	float totalTemp = 0, maxTemp = 0, minTemp = FLT_MAX;
	int totalFound = 0;
	char ** results = (char**)malloc(4);
	float values[STRUCT_SIZE] = { 0 };

	for (int t = startTime; t <= endTime; t++) {
		for (int i = 0; i < btIndex; i++) {
			if (bt[i].time == t) {
				totalTemp += bt[i].value;
				values[totalFound++] = bt[i].value;

				// max min 
				if (bt[i].value > maxTemp) {
					maxTemp = bt[i].value;
				}
				else if (bt[i].value < minTemp) {
					minTemp = bt[i].value;
				}
			}
		}
	}

	if (totalFound == 0) {
		free(results);
		return NULL;
	}

	float average = totalTemp / totalFound;
	float stdDev = getStdDev(values, totalFound, average);

	results[STAT_MIN] = (char*)malloc(10);
	results[STAT_MAX] = (char*)malloc(10);
	results[STAT_AVERAGE] = (char*)malloc(10);
	results[STAT_DEVIATION] = (char*)malloc(10);
	snprintf(results[STAT_MIN], 10, "%f", minTemp);
	snprintf(results[STAT_MAX], 10, "%f", maxTemp);
	snprintf(results[STAT_AVERAGE], 10, "%f", average);
	snprintf(results[STAT_DEVIATION], 10, "%f", stdDev);
	
	return results;
}


char** getPressStats(int startTime, int endTime) {
	int totalFound = 0, minBpd = INT_MAX, minBps = INT_MAX, maxBpd = 0, maxBps = 0;
	float totalBpd = 0, totalBps = 0;
	char **results = (char**)malloc(4);
	float bpdVals[STRUCT_SIZE] = { 0 };
	float bpsVals[STRUCT_SIZE] = { 0 };

	for (int t = startTime; t <= endTime; t++) {
		for (int i = 0; i < btIndex; i++) {
			if (bp[i].time == t) {
				totalBpd += bp[i].bpd;
				totalBps += bp[i].bps;
				bpdVals[totalFound] = (float)bp[i].bpd;
				bpsVals[totalFound++] = (float)bp[i].bps;

				// max min
				if (bp[i].bpd > maxBpd) {
					maxBpd = bp[i].bpd;
				}
				else if (bp[i].bpd < minBpd) {
					minBpd = bp[i].bpd;
				}

				if (bp[i].bps > maxBps) {
					maxBps = bp[i].bps;
				}
				else if (bp[i].bps < minBps) {
					minBps = bp[i].bps;
				}
			}
		}
	}

	if (totalFound == 0) {
		free(results);
		return NULL;
	}

	float averageBpd = totalBpd / totalFound;
	float averageBps = totalBps / totalFound;
	float stdDevBpd = getStdDev(bpdVals, totalFound, averageBpd);
	float stdDevBps = getStdDev(bpsVals, totalFound, averageBps);

	results[STAT_MIN_BPD] = (char*)malloc(10);
	results[STAT_MAX_BPD] = (char*)malloc(10);
	results[STAT_AVERAGE_BPD] = (char*)malloc(10);
	results[STAT_DEVIATION_BPD] = (char*)malloc(10);
	results[STAT_MIN_BPS] = (char*)malloc(10);
	results[STAT_MAX_BPS] = (char*)malloc(10);
	results[STAT_AVERAGE_BPS] = (char*)malloc(10);
	results[STAT_DEVIATION_BPS] = (char*)malloc(10);
	snprintf(results[STAT_MIN_BPD], 10, "%d", minBpd);
	snprintf(results[STAT_MAX_BPD], 10, "%d", maxBpd);
	snprintf(results[STAT_AVERAGE_BPD], 10, "%f", averageBpd);
	snprintf(results[STAT_DEVIATION_BPD], 10, "%f", stdDevBpd);
	snprintf(results[STAT_MIN_BPS], 10, "%d", minBps);
	snprintf(results[STAT_MAX_BPS], 10, "%d", maxBps);
	snprintf(results[STAT_AVERAGE_BPS], 10, "%f", averageBps);
	snprintf(results[STAT_DEVIATION_BPS], 10, "%f", stdDevBps);

	return results;
}


char** getRateStats(int startTime, int endTime) {
	float totalHeartRate = 0, maxRate = 0, minRate = FLT_MIN;
	int totalFound = 0;
	char ** results = (char**)malloc(4);
	float values[STRUCT_SIZE] = { 0 };

	for (int t = startTime; t <= endTime; t++) {
		for (int i = 0; i < btIndex; i++) {
			if (hr[i].time == t) {
				totalHeartRate += hr[i].value;
				values[totalFound++] = hr[i].value;

				// max min 
				if (hr[i].value > maxRate) {
					maxRate = hr[i].value;
				}
				else if (hr[i].value < minRate) {
					minRate = hr[i].value;
				}
			}
		}
	}

	if (totalFound == 0) {
		free(results);
		return NULL;
	}

	float average = totalHeartRate / totalFound;
	float stdDev = getStdDev(values, totalFound, average);

	results[STAT_MIN] = (char*)malloc(10);
	results[STAT_MAX] = (char*)malloc(10);
	results[STAT_AVERAGE] = (char*)malloc(10);
	results[STAT_DEVIATION] = (char*)malloc(10);
	snprintf(results[STAT_MIN], 10, "%f", minRate);
	snprintf(results[STAT_MAX], 10, "%f", maxRate);
	snprintf(results[STAT_AVERAGE], 10, "%f", average);
	snprintf(results[STAT_DEVIATION], 10, "%f", stdDev);

	return results;
}


void getStatValues(int startTime, int endTime) {
	// calculate 
	char** temp = getTempStats(startTime, endTime);
	char** rate = getRateStats(startTime, endTime);
	char** press = getPressStats(startTime, endTime);

	// display
	if (temp == NULL || press == NULL || rate == NULL) {
		printf("Error: cannot find data for interval [%d, %d]\n", startTime, endTime);
		return;
	}

	printf("%s\n", "--------------------------------------------------------");
	printf("      Statistical Values between time %d and %d\n", startTime, endTime);
	printf("%s\n", "---------+-----------+---------------------+------------");
	printf("%s\n", "Variable + Body Temp + Blood Pressure      + Heart Rate ");
	printf("%s\n", "---------+-----------+---------------------+------------");
	printf("%8s |%6.2fF%4c|%6.1f -%6.1f mm Hg | %5.1f bpm\n", 
		"Min", 
		(float)atof(temp[STAT_MIN]), ' ', 
		(float)atof(press[STAT_MIN_BPD]),
		(float)atof(press[STAT_MIN_BPS]),
		(float)atof(rate[STAT_MIN]));
	printf("%8s |%6.2fF%4c|%6.1f -%6.1f mm Hg | %5.1f bpm\n", 
		"Max", 
		(float)atof(temp[STAT_MAX]), ' ',
		(float)atof(press[STAT_MAX_BPD]),
		(float)atof(press[STAT_MAX_BPS]),
		(float)atof(rate[STAT_MAX]));
	printf("%8s |%6.2fF%4c|%6.1f -%6.1f mm Hg | %5.1f bpm\n", 
		"Average", 
		(float)atof(temp[STAT_AVERAGE]), ' ',
		(float)atof(press[STAT_AVERAGE_BPD]),
		(float)atof(press[STAT_AVERAGE_BPS]),
		(float)atof(rate[STAT_AVERAGE]));
	printf("%8s |%6.2fF%4c|%6.1f -%6.1f mm Hg | %5.1f bpm\n", 
		"StdDev", 
		(float)atof(temp[STAT_DEVIATION]), ' ',
		(float)atof(press[STAT_DEVIATION_BPD]),
		(float)atof(press[STAT_DEVIATION_BPS]),
		(float)atof(rate[STAT_DEVIATION]));
	printf("%s\n", "---------+-----------+---------------------+------------");
}


int main(int argc, const char** argv) {
	char menuOption = 0;
	char* fileLocation = NULL;
	int hasFileLocation = false, hasTimes = false;
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
				startTime = getTime("start ", MIN_TIME);
				endTime = getTime("end ", startTime);
				hasTimes = true;
				printf("%s\n", "Complete updating time interval");
				break;
			case 'c':
			case 'C':
				if (!hasFileLocation) {
					printf("%s\n", "Error: no data");
				}
				else {
					time = getTime("", MIN_TIME);
					printf("\n");
					getHealthScore(time);
				}
				break;
			case 'd':
			case 'D':
				if (!hasFileLocation) {
					printf("%s\n", "Error: no data");
				}
				else if (!hasTimes) {
					printf("%s\n", "Error: no time interval");
				}
				else {
					getStatValues(startTime, endTime);
				}
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
