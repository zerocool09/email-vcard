/******************************************************************************
Student Name: Ben Ventresca			Student Number: 0823887
Date: Feb 10th, 2014				Course Name: CIS-2750
Contact: bventres@uoguelph.ca		Description: Vcutil.c utilites functions
*****************************************************************************/
#define _GNU_SOURCE

#include <ctype.h>
#include "vcutil.h"

/*this function trims whitespace off of values, 
especially important for comparing values and strings*/
char *trimWhiteSpace(char *str){
	char *end;
 
	//trim spaces before
	while(isspace(*str)){
		str++;
	}
	if(*str == 0){
		return str;
	}
	//trim spaces after the line
	end = str + strlen(str) - 1;
	while(end > str && isspace(*end)){
		end--;
	}
	//write the null terminator
	*(end+1) = '\0';
	
	return str;
}

/*converts strings to completely upper case*/
char * stringToUpper(char *string){
	int i = 0;
	char * upperCase = malloc(sizeof(char)*strlen(string)+1);
	assert(upperCase);
	for(i = 0; string[i]; i++){//loop through
		upperCase[i] = toupper(string[i]);//convert case
	}
	
	upperCase[i] = '\0';
	return upperCase;
}

//for printing the properties of a vcard
void printVcProp(VcProp * propp){
	
	printf("Property ID/name is %d\n", propp->name);
	printf("Property Types: %s\n", propp->partype);
	printf("Property type value: %s\n", propp->parval);
	printf("Property VALUE: %s\n", propp->value);
}

//print all the properties from a file pointer
void printAll(VcFile * filep){
		int i = 0, j = 0;
		
		for(i = 0; i < filep->ncards; i++){
			for(j = 0; j < filep->cardp[i]->nprops; j++){
				printf("CARD #%d\n", i);
				printVcProp(&filep->cardp[i]->prop[j]);//call function print the properties
				printf("**********************************************\n");
			}
		}
}

/*helper function to parseVcProp, does return VcErrors
this checks for syntax and parover errors in vcard*/
VcError parsePropHelper(VcProp *const propp, char * token, const char *buff){
	
	char * selectionEnums[ARRAY_SIZE];
	char * parsedType = malloc(sizeof(char)*strlen(token));
	char * parsedValue = malloc(sizeof(char)*strlen(token));
	char * remainingInfo = malloc(sizeof(char)*strlen(token));
	assert(parsedType);
	assert(parsedValue);
	assert(remainingInfo);
	remainingInfo[0] = '\0'; //initiliaze the string
	char * string = malloc(sizeof(char)*strlen(token));
	assert(string);
	int i = 0, foundName = 0, numOfTypes = 0, numOfValues = 0;
	int numUnusedParams = 0;
	
	selectionEnums[0] = "BEGIN";
	selectionEnums[1] = "END";
	selectionEnums[2] = "VERSION";
	selectionEnums[3] = "N";
	selectionEnums[4] = "FN";
	selectionEnums[5] = "NICKNAME";
	selectionEnums[6] = "PHOTO";
	selectionEnums[7] = "BDAY";
	selectionEnums[8] = "ADR";
	selectionEnums[9] = "LABEL";
	selectionEnums[10] = "TEL";
	selectionEnums[11] = "EMAIL";
	selectionEnums[12] = "GEO";
	selectionEnums[13] = "TITLE";
	selectionEnums[14] = "ORG";
	selectionEnums[15] = "NOTE";
	selectionEnums[16] = "UID";
	selectionEnums[17] = "URL";
	selectionEnums[18] = "OTHER";
	 
	//allocate for the partype and parval parameters
	propp->partype = malloc(sizeof(char)*strlen(buff)+1);
	propp->parval = malloc(sizeof(char)*strlen(buff)+1);
	assert(propp->partype);
	assert(propp->parval);
	
	string = strtok(token, ";");
	string = stringToUpper(string);
	//printf("our string is: %s\n", string);

	for(i = 0; i < strlen(string); i++){//check for periods loop
		if ( string[i] == '.' ){
			strcpy(string, string+i+1);//copy the new string
			//printf("FOUND A PERIOD! new string is: %s\n", string);
			break;
		}
	}

	//printf("parHELPER 2:. AM I SEGFAULTING HERE?!?!?\n");
	for(i = 0; i < ARRAY_SIZE; i++){
		
		//check the possible IDs we could get
		if ( (strstr(string, selectionEnums[i]) && strlen(string) == strlen(selectionEnums[i])) ){
			
			foundName = 1;
			while( (string = strtok(NULL, ";")) ){
				strcat(remainingInfo, string);//concatenate the remaining info after the name
				strcat(remainingInfo, ";"); //make sure the formatting is correct
			}

			propp->name = i;//set name
			token = strtok(remainingInfo, ";");
			//loop while we still have type or value parameters
			//(strcasestr(remainingInfo, "TYPE=") || strcasestr(remainingInfo, "VALUE=") ) && 
			while( (token != NULL)){
				//printf("parHELPER 3:. AM I SEGFAULTING HERE?!?!?\n");
				if (strcasestr(token, "TYPE=") ){//if there is just type in the string
					numOfTypes += 1;
					strcpy(parsedType, token+5); //get the data and remove the type tag
					if (strlen(parsedType) == 0){
					//printf("SYNTAX ERROR\n");
						return SYNTAX;
					}
					if (numOfTypes <= 1){
						strcpy(propp->partype, parsedType);//set the parsed data for type
					}
					else{
						strcat(propp->partype, ",");
						strcat(propp->partype, parsedType);
					}
				}
				else if( strcasestr(token, "VALUE=") ){ //if there is just value in the str
					numOfValues += 1;
					strcpy(parsedValue, token+6);//get the data and remove the type tag
					if(numOfValues <= 1){
						strcpy(propp->parval, parsedValue);
					}
					else{
						strcat(propp->parval, ",");
						strcat(propp->parval, parsedValue);
					}
				}
				else if (strcasestr(token, "CHARSET=") || strcasestr(token, "ENCODING=") ){
					numUnusedParams += 1;
				}
				else{
					//printf("GOT INTO ELSE\n");
					if (strcasestr(token, "TYPE") || strcasestr(token, "VALUE")){
						//printf("SYNTAX ERROR\n");
						return SYNTAX;
					}
					else{
						//printf("PAROVER ERROR\n");
						return PAROVER;//return parover error 
					}
				}
				token = strtok(NULL, ";");
			}
			break;
		}
	}

	if (numOfTypes == 0){//if there was no types found
		propp->partype = NULL;
	}
	if (numOfValues == 0){//if there was no values found
		propp->parval = NULL;
	}

	//this is if we do not get any of the above properties, we place it an an OTHER property
	if (foundName == 0){
		strtok(token, ";");
		if(strlen(token) == 0){
			return SYNTAX;
		}
		else{
			propp->name = VCP_OTHER;//set it to an OTHER property
			propp->partype = NULL;
			propp->parval = NULL;
			propp->hook = NULL;
			
			free(propp->value);
			propp->value = malloc(sizeof(char)*strlen(buff)+1);
			assert(propp->value);
			strcpy(propp->value, buff);
		}
	}
	
	free(parsedType);
	free(parsedValue);
	free(remainingInfo);
	free(string);
	
	return OK;
}


VcStatus writeVcFile( FILE* const vcf, const VcFile *filep ){

	VcStatus newStatus;
	newStatus.code = OK;
	newStatus.lineto = 0;
	newStatus.linefrom = 0;
	int i = 0, j = 0, k = 0, stringLen = 0, firstLen = 0;
	int wrapCount = 1;
	char * selectionEnums[ARRAY_SIZE];
	selectionEnums[0] = "BEGIN";
	selectionEnums[1] = "END";
	selectionEnums[2] = "VERSION";
	selectionEnums[3] = "N";
	selectionEnums[4] = "FN";
	selectionEnums[5] = "NICKNAME";
	selectionEnums[6] = "PHOTO";
	selectionEnums[7] = "BDAY";
	selectionEnums[8] = "ADR";
	selectionEnums[9] = "LABEL";
	selectionEnums[10] = "TEL";
	selectionEnums[11] = "EMAIL";
	selectionEnums[12] = "GEO";
	selectionEnums[13] = "TITLE";
	selectionEnums[14] = "ORG";
	selectionEnums[15] = "NOTE";
	selectionEnums[16] = "UID";
	selectionEnums[17] = "URL";
	selectionEnums[18] = "OTHER";

	if (filep->ncards == 0){
		//printf("WE DON'T HAVE ANY CARDS TO WORK WITH\n");
	}
	else{
		//printf("GOT %d cards\n", filep->ncards);
		for(i = 0; i < filep->ncards; i++){//loop through number of cards
			if (fprintf(vcf, "BEGIN:VCARD\r\nVERSION:%s\r\n", VCARD_VER) < 0){//place begins and version in
				newStatus.code = IOERR;
				return newStatus;
			}
			newStatus.lineto += 2;
			newStatus.linefrom += 2;
			
			for(j = 0; j < filep->cardp[i]->nprops; j++){//loop through properties

				firstLen += strlen(selectionEnums[filep->cardp[i]->prop[j].name]);
				if (filep->cardp[i]->prop[j].name != VCP_OTHER){
					if ( fprintf(vcf, "%s", selectionEnums[filep->cardp[i]->prop[j].name]) < 0){
						newStatus.code = IOERR;
						return newStatus;
					}
				}
				if(filep->cardp[i]->prop[j].partype != NULL){//check if there is a type to write
					if ( fprintf(vcf, ";TYPE=%s", filep->cardp[i]->prop[j].partype) < 0){
						newStatus.code = IOERR;
						return newStatus;
					}
					firstLen += (strlen(filep->cardp[i]->prop[j].partype) + 6);
				}
				if(filep->cardp[i]->prop[j].parval != NULL){
					if ( fprintf(vcf, ";VALUE=%s", filep->cardp[i]->prop[j].parval) < 0){
						newStatus.code = IOERR;
						return newStatus;
					}
					firstLen += (strlen(filep->cardp[i]->prop[j].parval) + 7);
				}
				stringLen += (strlen(filep->cardp[i]->prop[j].value) + firstLen);// FIXTAG +3 was here

				//check for folding lengths
				if(stringLen > FOLD_LEN){
					wrapCount = firstLen;
					for (k = 0; k < stringLen - firstLen; k++){
						if (wrapCount == 75){
							//printf("GOT IN HERE\n");
							fprintf(vcf, "\r\n ");
							newStatus.lineto += 1;
							newStatus.linefrom += 1;
							wrapCount = 1;
						}
						if (k == 0){
							fprintf(vcf, ":");
						}
						//printf("Wrap count %d\n", wrapCount);
						fprintf(vcf, "%c", filep->cardp[i]->prop[j].value[k]);//place character in file
						wrapCount += 1;
					}
					fprintf(vcf, "\r\n");
					newStatus.lineto += 1;
					newStatus.linefrom += 1;
				}
				else{
					if (filep->cardp[i]->prop[j].name != VCP_OTHER ){
						if ( fprintf(vcf, ":%s\r\n", filep->cardp[i]->prop[j].value) < 0){//write the value
							newStatus.code = IOERR;
							return newStatus;
						}
					}
					else{
						if ( fprintf(vcf, "%s\r\n", filep->cardp[i]->prop[j].value) < 0){//write the value
							newStatus.code = IOERR;
							return newStatus;
						}
					}
					newStatus.lineto += 1;
					newStatus.linefrom += 1;
				}
				firstLen = 0;	
				stringLen = 0;
			}
			
			newStatus.lineto += 1;
			newStatus.linefrom += 1;
			if ( fprintf(vcf, "END:VCARD\r\n") < 0){//write end of vcard
				newStatus.code = IOERR;
				return newStatus;
			}
			
		}
		

	}
	//printf("line numbers are: %d, %d\n", newStatus.lineto, newStatus.linefrom);
	return newStatus;
}


/*readVcFile takes in the file that is open and a variable for a constant file pointer
this function goes through files and reads all the legitimate vcards and gets any error or
OK statuses back. This function frees all the vcards if there is an error code returned*/
VcStatus readVcFile( FILE *const vcf, VcFile *const filep){
	
	//printf("VCUTIL: read called\n");

	int i = 0;
	VcStatus newStatus;
	newStatus.code = OK;
	
	filep->ncards = 0;
	filep->cardp = NULL;
	filep->cardp = malloc(sizeof(Vcard*)*HUND_SIZE);
	assert(filep->cardp);
	
	while( newStatus.code == OK ){//FIXTAG. newStatus.code == OK && (filep->cardp[i]) != NULL
		newStatus = readVcard(vcf, &filep->cardp[i]);
		filep->ncards += 1;
		if (filep->cardp[i] == NULL){//if the cardp was null then we break
			//printf("TRUE\n");
			break;
		}
		//printf("NUM OF CARDS= %d\n", filep->ncards);
		i++;
	}
	if (filep->ncards > 1 || i == 0){
		filep->ncards -= 1;//make sure the number of cards is not off by 1
	}
	if (newStatus.code == OK && i >= 1){//if we didnt report errors, then
		newStatus.lineto -= 2;//fix the offset of linenumbers
		newStatus.linefrom -= 2;
	}
	//printf("line numbers, lineto: %d, linefrom: %d\n", newStatus.lineto, newStatus.linefrom);

	if (newStatus.code != OK){//if we didn't get an OK status, free all cards
		freeVcFile(filep);//FIXTAG, call free
	}
	//printAll(filep);
	//freeVcFile(filep);
	//printf("THE number of times we looped was %d\n", i);
	//printf("The number of cards we got was %d\n", filep->ncards);
	
	return newStatus;
}

/*readVcard takes in the open file for read and a card pointer 
it then goes through checking for the beginning of a vcard, 
then following the specifications of the format of version 3.0 vcards
if everything goes well it returns OK and returns to caller.
If there is an error it returns the error status to caller*/
VcStatus readVcard( FILE *const vcf, Vcard **const cardp){
	VcStatus newStatus;
	newStatus.code = OK;
	newStatus.linefrom = 0;
	newStatus.lineto = 0;
	
	VcProp newProp;
	newProp.name = VCP_OTHER; //initialize values
	newProp.partype = NULL;//FIXTAG, changed from "" to NULL
	newProp.parval = NULL;
	newProp.value = NULL;
	
	VcError errorCode;//for error codes
	
	int gotBegins = 0, gotVersion = 0, gotN = 0, gotFN = 0, statBegins = 0;
	char * buff = malloc(sizeof(char)*LINE_SIZE);
	assert(buff);
	
	*cardp = malloc(sizeof(Vcard)*100);
	assert(cardp);
	(*cardp)->nprops = 0;

	//printf("BUFF BEFORE LOOP= %s:\n", buff);
	//((*cardp)->prop.name[i-1]
	while( newProp.name != VCP_END || (buff != NULL && (strlen(buff) != 0)) ){//FIXTAG, use || buff != NULL

		//printf("BUFF BEFORE GETUNFOLD = %s\n, status is %d\n", buff, newStatus.code);
		newStatus = getUnfolded(vcf, &buff);
		/*if(buff != NULL){
			printf("BUFF AFTER = %s\n, status is = %d\n", buff, newStatus.code);	
		}
		else{
			printf("BUFF GOT FREED\n");
		}
			
		printf("lineto %d, linefrom %d\n", newStatus.lineto, newStatus.linefrom);*/

		if (buff != NULL && (strlen(buff) != 0)){//THERE IS AN ISSUE WITH THIS
			//printf("READVCARD: send to parse now!\n");
			errorCode = parseVcProp(buff, &newProp);
			if(errorCode){//printf("we got an error\n");
				newStatus.code = errorCode;
				//printf("2) ERROR CODE, so IS: %d\n", newStatus.code);
				return newStatus;
			}
			//printf("AFTER PARSE: id: %d\n", newProp.name);
		}
		else if( newStatus.lineto == 0 ){//FIXTAG
			(*cardp) = NULL;//set card to NULL then 
			newStatus.code = OK;
			return newStatus;
		}
		else {//if we didnt get an end:vcard
			//break;//FIXTAG MARK 1 TESTS, BUT THIS MESSED UP MARK 2 TESTS!
			//printf("STATUS before FINAL CHECK: %d\n", newStatus.code);
			if (newProp.name == VCP_END || newProp.name == VCP_OTHER){
				break;
			}
			else{
				//printf("INFO: id: %d, lineto: %d, linefrom: %d, code: %d\n", newProp.name, newStatus.lineto, newStatus.linefrom, newStatus.code);
				newStatus.code = BEGEND;//end error
				return newStatus;
			}
		}
		
		if(gotBegins == 1){//if we have seen begins
			statBegins = 1;
			if(newProp.name == VCP_VERSION){//then check version comes after it
				gotVersion = 1;
				gotBegins = 0;//reset begins boolean
				if (strcmp(newProp.value, VCARD_VER) != 0){//if the version doesnt match
					//printf("BAD VERSION, value = %s, version = %s\n", newProp.value, VCARD_VER);
					newStatus.code = BADVER;//we got a wrong version
					return newStatus;
				}
			}
			else{//we got no version after begin:
				//printf("NO VERSION ERROR\n");
				newStatus.code = NOPVER;//no version error
				return newStatus;
			}
		}
		//(newStatus.linefrom == 0) && 
		if( (newProp.name == VCP_BEGIN) ){//if we got begin:vcard
			gotBegins = 1;
		}

		if(newProp.name == VCP_N){//if we got a name
			gotN = 1;
		}
		if(newProp.name == VCP_FN){//if we got a full name
			gotFN = 1;
		}
		
		//if we have reached the end of the vcard
		if(newProp.name == VCP_END){
			if(gotN == 0 || gotFN == 0){//check if we got a full name and name
				//printf("NAME OR FULLNAME ERROR\n");
				newStatus.code = NOPNFN;
				return newStatus;
			}
			else if (statBegins == 0){//final check if we got a begins
				newStatus.code = BEGEND;
				return newStatus;
			}
			else if (gotVersion == 0){//final check if we got a version
				newStatus.code = NOPVER;
				return newStatus;
			}
			else{//if all is good return the okay status.
				return newStatus;
			}
		}
		
		//if its not a begin, end or version for vcard then add it to the properties
		if( (newProp.name != VCP_BEGIN) && (newProp.name != VCP_END) && (newProp.name != VCP_VERSION) ){
			//incremement the number of properties we have in our cardp
			(*cardp)->nprops += 1;
			(*cardp)->prop[(*cardp)->nprops-1] = newProp;//set the cardp property array to take this newproperty
			//printVcProp(&(*cardp)->prop[(*cardp)->nprops - 1]);
			//printf("the num of properties: %d **********************************************************\n\n", (*cardp)->nprops);
		}
		
	}
	
	//if (buff != NULL){
	if (feof(vcf)){
		//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!REACHED END OF FILE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		(*cardp) = NULL;
		//newStatus = getUnfolded(NULL, &buff);//FIXTAG TO GET LINE NUMBERS INSTEAD OF A ZERO AT END OF FILE
		newStatus.code = OK;
		free(buff);
		buff = NULL;
		return newStatus;
	}
	//printf("NUM OF CARD PROPERTIES = %d\n", (*cardp)->nprops);
	
	return newStatus;
}

/*Getunfolded function takes in the open file and a constant buffer
it reads lines from the open file and checks if they are wrapped
if ANY lines are wrapped it reassembles them into 1 line and places
the unwrapped line into the buffer.*/
VcStatus getUnfolded( FILE *const vcf, char **const buff ){
	
	static char * lineBuffer;
	char * beginning = malloc(sizeof(char)*LINE_SIZE);
	char * editedLine = malloc(sizeof(char)*LINE_SIZE);
	assert(beginning);
	assert(editedLine);
	char * flag;
	static int endOfFile = 0;
	static int lineNumber = 1;
	int i = 0, length = 0, spaceCount = 0;
	
	VcStatus newStatus;
	if (vcf == NULL){
		//printf("FILE IS NULL\n");
		lineNumber = 1;
		
		newStatus.code = 0;
		newStatus.linefrom = 0;
		newStatus.lineto = 0;
		
		free(beginning);
		free(editedLine);
		
		return newStatus;
	}
	newStatus.linefrom = lineNumber;
	//printf("END OF FILE NUM IS %d\n", endOfFile);
	if (lineNumber == 1){//if we're at the first line
		//printf("**************FIRST LINE************************\n");
		lineBuffer = malloc(sizeof(char)*LINE_SIZE);//malloc for the line buffer
		assert(lineBuffer);
		flag = fgets(lineBuffer, LINE_SIZE, vcf);
		endOfFile = 0;//FIXTAG
	}
	//printf("1) LINE= %s:\n", lineBuffer);
	while(strlen(lineBuffer) < 2 && lineNumber != 1){//if the line is less than 2 characters long and not the first line
		flag = fgets(lineBuffer, LINE_SIZE, vcf);
		lineNumber += 1;
	}
	//printf("2) LINE= %s:\n", lineBuffer);
	if( (strlen(lineBuffer) >= 2) || ((flag = fgets(lineBuffer, LINE_SIZE, vcf)) != NULL) ){
		//printf("3) LINE= %s:\n", lineBuffer);
		for(i = 0; i < strlen(lineBuffer); i++){
			if(lineBuffer[i] == '	' || lineBuffer[i] == ' '){
				spaceCount += 1;
				if (spaceCount == strlen(lineBuffer)-2){
					//printf("GOT A BLANK LINE\n");
					if((flag = fgets(lineBuffer, LINE_SIZE, vcf)) != NULL){
						//printf("4) LINE= %s:\n", lineBuffer);
						strcpy(beginning, lineBuffer);
						lineNumber += 1;
						spaceCount = 0;//added this to try and FIXTAG
					}
					break;
				}
			}
			else{
				break;
			}
		}
		if (spaceCount >= 1 && ( (strlen(lineBuffer)-2) != spaceCount )){
			//printf("!*!*!*!*!*!*!*!*!*!*!beginning before is = %s:\n", beginning);
			//printf("WE GOT INFO AFTER A COUPLE SPACES/TABS\n");
			//if((flag = fgets(lineBuffer, 999, vcf)) != NULL){
			strcpy(beginning, lineBuffer+spaceCount);
			//lineNumber += 1;
			//}
			//printf("!*!*!*!*!*!*!*!*!*!*!beginning AFTER is = %s\n", beginning);
		}
		//printf("5) LINE= %s:\n", lineBuffer);
		strcpy(beginning, lineBuffer);
		beginning[strcspn(beginning, "\r\n")] = '\0';
		lineNumber += 1;

		//printf("!*!*!beginning = %s\n", beginning);
		
		//checking for wrapping lines and putting them together
		while( ((flag = fgets(lineBuffer, LINE_SIZE, vcf)) != NULL) && (lineBuffer[0] == ' ' || (( (lineBuffer[0] == '\n') 
			|| (lineBuffer[0] == '\t') || (lineBuffer[0] == '\v') || (lineBuffer[0] == '\f') 
			|| (lineBuffer[0] == '\r')) ) ) ){
			//printf("GOT INTO WHILE LOOP ******************!!!!!!!!!!!!!!!!!!!!!!!\n");
			//printf("6) LINE= %s:\n", lineBuffer);
			strcpy(editedLine, lineBuffer);
			length = strlen(editedLine);
			for(i = 0; i < length; i++){
				if ( ((editedLine[i] != ' ') || (i >= 1)) && ((editedLine[i] != '\t') || (i >= 1) )){//FIXTAG and && (i <= 2) 
					editedLine[strcspn(editedLine, "\r\n")] = '\0';
					strcat(beginning, editedLine+i);//concatenate the wrapped lines together
					//printf("CONCATED LINE= %s: i= %d, endofile is %d\n", beginning, i, endOfFile);
					lineNumber += 1;
					break;
				}
			}
		}
	}
 
	//if we reach the end of the file, then do these checks
	if (feof(vcf) && !(strcasestr("END:VCARD", lineBuffer)) && endOfFile == 1){
		//printf("!!!!!!GOT TO END OF FILE, BUFF: %s, LINE#: %d\n", *buff, lineNumber);
		*buff = NULL;
		newStatus.linefrom = newStatus.lineto = lineNumber;
		endOfFile = 0;//reset it
		return newStatus;
	}
	else if (feof(vcf) && !(strcasestr("END:VCARD", lineBuffer))){
		endOfFile = 1;
	}
	
	newStatus.code = 0;
	newStatus.lineto = lineNumber-1;

	//printf("ALL INFO before strcpy buff: %s: code: %d, linefrom: %d, lineto: %d\n", beginning, newStatus.code, newStatus.linefrom, newStatus.lineto);
	
	*buff = malloc(sizeof(char)*strlen(beginning)+1);
	assert(buff);
	if (strcmp(beginning, "") == 0){
		*buff = NULL;
		//printf("GOT BEGINNING THAT HAD NOTHING\n");
	}
	else{
		strcpy(*buff, beginning);
	}
	
	//free(beginning);
	//free(editedLine);
	//free(lineBuffer);
	//printf("*************************************************\n");
	
	return newStatus;
}

/*parseVcProp function takes in a buffer that has been unfolded by getUnfolded
and it takes in a property that we will be adding to.
IF the format is correct and no errors are returned, then we will successfully create
a new property for a vcard and place the property in the data structure*/
VcError parseVcProp( const char * buff, VcProp *const propp){
	
	char * token = NULL;
	VcError code = OK;
	char * parseBuffer = malloc(sizeof(char)*strlen(buff)+1);
	char * firstHalf = malloc(sizeof(char)*strlen(buff)+1);
	char * value = malloc(sizeof(char)*strlen(buff)+1);
	assert(parseBuffer);
	assert(firstHalf);
	assert(value);
	int badSyntax = 0;
	strcpy(parseBuffer, buff);
	
	propp->name = 0;
	propp->partype = NULL;//initialize property values
	propp->parval = NULL;
	propp->value = NULL;
	propp->hook = NULL;
	
	//printf("PARSE 1:. AM I SEGFAULTING HERE?!?!?\n");
	
	token = strtok(parseBuffer, ":");
	if (token == NULL || strlen(token) == 0){
		badSyntax = 1;
	}
	else{
		strcpy(firstHalf, token);
	}
	
	//printf("PARSE 2:. AM I SEGFAULTING HERE?!?!?\n");
	token = strtok(NULL, "\n");
	if (token == NULL || badSyntax == 1){
		badSyntax = 1;
	}
	else{
		strcpy(value, token);
		propp->value = malloc(sizeof(char)*strlen(value)+1);
		assert(propp->value);
		
		value = trimWhiteSpace(value);
		strcpy(propp->value, value);
		//printf("PARSE 3:. AM I SEGFAULTING HERE?!?!?\n");
		code = parsePropHelper(propp, firstHalf, buff);
	}

	if(badSyntax == 1){
		//printf("WE GOT BAD SYNTAX IN PARSE\n");
		free(parseBuffer);
		//parseBuffer = NULL;
		free(firstHalf);
		//firstHalf = NULL;
		free(value);
		//value = NULL;
		return SYNTAX;
	}
	else{
		//printf("PARSE 4:. AM I SEGFAULTING HERE?!?!?\n");
	}
	return code;
}
 
/*freeVcFile takes a filer pointer and loops through all the vcards
and it frees all its properties*/
void freeVcFile ( VcFile *const filep){
	int i = 0, j = 0;
	if ( filep->cardp != NULL){
		for(i = 0; i < filep->ncards; i++){
			for(j = 0; j < filep->cardp[i]->nprops; j++){
				if ( filep->cardp[i]->prop[j].parval != NULL){
					free(filep->cardp[i]->prop[j].parval);//free the parval property
				}
				if ( filep->cardp[i]->prop[j].partype != NULL){
					free(filep->cardp[i]->prop[j].partype);
				}
				if ( filep->cardp[i]->prop[j].value != NULL){
					free(filep->cardp[i]->prop[j].value);
				}
			}
			if (filep->cardp != NULL){
				free(filep->cardp);
			}
		}
	}
}