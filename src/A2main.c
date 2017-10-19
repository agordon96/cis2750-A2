#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "CalendarParser.h"
#include "Helpers.h"

int main(int argc, char *argv[]) {
	char *toPrint;
  char choice[256];
  char fileName[256];
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  Alarm *alarm;
	Calendar *cal = NULL;
  Calendar *tempCal = NULL;
  Event *event;
	ICalErrorCode err;
  
  while(true) {
    printf("\n1) Read iCal file\n2) Display calendar\n3) Create new calendar object\n");
    printf("4) Write calendar object to file\n5) Exit\n\n");

    scanf("%s", choice);
    if(strcmp(choice, "1") == 0) {
      while(true) {
        if(cal) {
          printf("Calendar already populated. Would you like to clear it and open another one?");
          printf(" Anything that isn't y will return to the main menu.\n");
          scanf("%s", choice);
          
          if(strcmp(choice, "y") == 0) {
            deleteCalendar(cal);
            cal = NULL;
          } else {
            break;
          }
        }
        
        printf("Please input a path string to the file to be parsed or \"exit\" to return to the menu.\n");
        scanf("%s", choice);
        
        if(strcmp(choice, "exit") == 0) {
          break;
        }
        
        err = createCalendar(choice, &cal);
        if(err != OK) {
          toPrint = printError(err);
          printf("\nError parsing file: %s\n", toPrint);
          
          free(toPrint);
          cal = NULL;
        } else {
          printf("File successfully parsed. Calendar can now be displayed.\n");
          break;;
        }
      }
    } else if(strcmp(choice, "2") == 0) {
      if(cal) {
        toPrint = printCalendar(cal);
        printf("%s\n", toPrint);
        free(toPrint);
      } else {
        printf("A file has not been successfully opened, parsed and saved yet.\n");
      }
    } else if(strcmp(choice, "3") == 0) {
      if(cal) {
        printf("\nCalendar already populated. Would you like to clear it and create a new custom one?");
        printf(" Anything that isn't y will return to the main menu.\n");
        scanf("%s", choice);
          
        if(strcmp(choice, "y") != 0) {
          continue;
        }
        
        deleteCalendar(cal);
        cal = NULL;
      }
      
      tempCal = (Calendar*)malloc(sizeof(Calendar));
      tempCal->version = 0.0;
      tempCal->properties.head = NULL;
      
      while(tempCal->version == 0.0) {
        printf("\n\nPlease enter a valid version number:\n");
        scanf("%s", choice);
        tempCal->version = atof(choice);
      }
      
      printf("\n\nEnter a product ID:\n");
      scanf("%s", choice);
      strcpy(tempCal->prodID, choice);
      
      tempCal->events = initializeList(&printFuncEvent, &deleteFuncEvent, &compareFuncEvent);
      event = (Event*)malloc(sizeof(Event));
      event->properties.head = NULL;
      
      printf("\n\nEnter UID for the event:\n");
      scanf("%s", choice);
      strcpy(event->UID, choice);
      
      strftime(event->creationDateTime.date, 100, "%Y%m%d", &tm);
      strftime(event->creationDateTime.time, 100, "%H%M%S", &tm);
      event->creationDateTime.UTC = 0;
      
      event->alarms = initializeList(&printFuncAlarm, &deleteFuncAlarm, &compareFuncAlarm);
      alarm = (Alarm*)malloc(sizeof(Alarm));
      alarm->properties.head = NULL;
      
      printf("\n\nEnter Trigger for the alarm:\n");
      scanf("%s", choice);
      alarm->trigger = (char*)malloc(sizeof(char) * (strlen(choice) + 1));
      strcpy(alarm->trigger, choice);
      
      printf("\n\nEnter Action for the event:\n");
      scanf("%s", choice);
      strcpy(alarm->action, choice);
      
      insertBack(&event->alarms, (void*)alarm);
      alarm = NULL;
      insertBack(&tempCal->events, (void*)event);
      event = NULL;
      
      err = validateCalendar(tempCal);
      if(err != OK) {
        toPrint = printError(err);
        printf("\nCalendar failed to validate: %s\n", toPrint);
        
        free(toPrint);
        deleteCalendar(tempCal);
      } else {
        cal = tempCal;
      }
      
      tempCal = NULL;
    } else if(strcmp(choice, "4") == 0) {
      if(cal) {
        printf("Please enter a file name.\n");
        scanf("%s", fileName);
        if(access(fileName, F_OK) != -1) {
          printf("\nFile already exists. Would you like to overwrite it with your custom calendar?");
          printf(" Anything that isn't y will return to the main menu.\n");
          scanf("%s", choice);
          
          if(strcmp(choice, "y") != 0) {
            continue;
          }
        }
        
        err = validateCalendar(cal);
        if(err == OK) {
          err = writeCalendar(fileName, cal);
        } else {
          toPrint = printError(err);
          printf("\nCalendar failed to validate: %s\n", toPrint);      
          free(toPrint);
          continue;
        }
        
        if(err != OK) {
          toPrint = printError(err);
          printf("\nError writing to file: %s\n", toPrint);      
          free(toPrint);
        } else {
          printf("\n%s successfully written. Calendar can now be displayed. Custom calendar cleared.\n", fileName);
          deleteCalendar(cal);
          cal = NULL;
        }
      } else {
        printf("Custom calendar has not been created yet.\n");
      }
    } else if(strcmp(choice, "5") == 0) {
      printf("Exiting...\n");
      break;
    } else {
      printf("Please enter a valid input number between 1 and 5.\n");
    }
  }
  
  if(cal) {
    deleteCalendar(cal);
  }

	return 0;
}