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
	for (int i = 0; i < strlen(str) - 1; i++) {
		if (isdigit((int)(str[i])) == 0) {
			return false;
		}
	}
	return true;
}


int isFloat(char* str) {
	int hasFoundDecimal = false;

	for (int i = 0; i < strlen(str); i++) {
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


int readFiles(const char* folder) {
	int status = false;
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
			bt[btIndex++].value = atof(token);
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
			hr[hrIndex++].value = atoi(token);
		}

		fclose(file);
		memset(fileName, 0, length);
	}

	free(fileName);
	printf("%s\n", "Finish populating data");

	return status;
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
	char *results[2];
	float score = 0;
	char* status = NULL;

	for (int i = 0; i < btIndex; i++) {
		if (bt[i].time == time) {
			totalTemp += bt[i].value;
			totalFound++;
		}
	}

	if (totalFound == 0) {
		return NULL;
	}

	float average = totalTemp / totalFound;

	if (97 <= average && average <= 99) {
		results[scoreValue] = itoa(1, buffer, 10);
		results[scoreStatus] = "Normal";
	}
	else {
		results[scoreValue] = itoa(0, buffer, 10);
		results[scoreStatus] = "Abnormal";
	}

	return results;
}


char** getPress(int time) {
	int totalFound = 0;
	float totalBpd = 0, totalBps = 0, score;
	char *results[2];
	char* status = NULL;

	for (int i = 0; i < btIndex; i++) {
		if (bp[i].time == time) {
			totalBpd += bp[i].bpd;
			totalBps += bp[i].bps;
			totalFound++;
		}
	}

	if (totalFound == 0) {
		return NULL;
	}

	if (totalBpd <= 80 && totalBps <= 120) {
		results[scoreValue] = itoa(1, buffer, 10);
		results[scoreStatus] = "Normal";
	}
	else if ((80 < totalBpd && totalBpd <= 89) ||
		(120 < totalBps && totalBps <= 139)) {
		fprintf(results[scoreValue], "%f", 0.5);
		results[scoreStatus] = "Prehypertension";
	}
	else if (totalBpd >= 90 || totalBps >= 140) {
		results[scoreValue] = itoa(0, buffer, 10);
		results[scoreStatus] = "Hypertension";
	}
	else {
		results[scoreValue] = itoa(INT_MIN, buffer, 10);
		results[scoreStatus] = "WTH";
	}

	return results;
}


char** getRate(int time) {
	float totalRate = 0;
	int totalFound = 0;
	char *results[2];
	float score = 0;
	char* status = NULL;

	for (int i = 0; i < btIndex; i++) {
		if (hr[i].time == time) {
			totalRate += hr[i].value;
			totalFound++;
		}
	}

	if (totalFound == 0) {
		return NULL;
	}

	float average = totalRate / totalFound;

	if (60 <= average && average <= 100) {
		results[scoreValue] = itoa(1, buffer, 10);
		results[scoreStatus] = "Normal";
	}
	else {
		results[scoreValue] = itoa(0, buffer, 10);
		if (average > 100) {
			results[scoreStatus] = "Tachycardia";
		}
		else {
			results[scoreStatus] = "Bradycardia";
		}
	}

	return results;
}


void getHealthScore(int time) {
	char** temp = getTemp(time);
	char** press = getPress(time);
	char** rate = getRate(time);

	if (temp == NULL || press == NULL || rate == NULL) {
		printf("Error: cannot find data for time %d\n", time);
		return;
	}

	float score = 20 * atof(temp[scoreValue]) + 25 * atof(press[scoreValue]) + 30 * atof(rate[scoreValue]) + 25;

	printf("%s\n", "Body Temperature     | Blood Pressure         | Heart Rate");
	printf("%s\n", "-------------------- + ---------------------- + ------------------");
	printf("%-4.1fF%24c|", 97.5, ' ');
	// 97.5F | 80 – 120 mm Hg | 58 bmp
	// Normal | Normal | Bradycardia
	printf("\n%s\n", "-------------------- + ---------------------- + ------------------");
	printf("At time %d, the Health Score was %.2f/100\n", time, score);
	printf("%s\n", "------------------------------------------------------------------");
}


void testPrint() {
	printf("%s\n", "Body Temperature     | Blood Pressure         | Heart Rate");
	printf("%s\n", "-------------------- + ---------------------- + ------------------");
	printf("%-4.1fF%16c|%24c|", 97.5, ' ', ' ');
	// 97.5F | 80 – 120 mm Hg | 58 bmp
	// Normal | Normal | Bradycardia
	printf("\n%s\n", "-------------------- + ---------------------- + ------------------");
	printf("At time %d, the Health Score was %.2f/100\n", 55, 70.0);
	printf("%s\n", "------------------------------------------------------------------");
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
