/********
vcftool.c -- vcftool.c in Asmt 2
Last updated:  Feb 10th, 2014 07:00:00 PM 

Student Name: Ben Ventresca			Student Number: 0823887
Date: Feb 10th, 2014					Course Name: CIS-2750
Contact: bventres@uoguelph.ca		Description: vcftools for assignment 2

***EXTRA DETAILS: iso3166-2_CountryCodes.c and .h files are cited from Przemyslaw Gawron
who generously posted this library on the moodle website for CIS-2750.
*****************************************************************************/

#include "vcftool.h"

int isupper(int c);
int toupper(int c);
int isdigit(int c);

/* This function does the comparison for qsort, and is 
used in the sort and info function
It takes in two void pointers and string compares their contents*/
int cardCmp(const void * card1, const void * card2){

	int i = 0, j = 0;
	int gotN1 = 0, gotN2 = 0;
	char * name1, * name2;
	char * token = NULL;
	char * firstName1, * lastName1;
	char * firstName2, * lastName2;

	const Vcard * one = (*(Vcard **)card1);//cast onto void pointers into vcards
	const Vcard * two = (*(Vcard **)card2);

	//printf("num of props is: card1: %d, card2: %d\n", (*one).nprops, (*two).nprops);

	for (i = 0; i < (*one).nprops; i++){//loop through properties of first card
		if ((*one).prop[i].name == VCP_N && gotN1 == 0){//look for the first instance of full name
			name1 = malloc(sizeof(char*)*strlen((*one).prop[i].value)+1);
			assert(name1);
			strcpy(name1, (*one).prop[i].value);//keep the full name
			//printf("NAME IS: %s, copied from %s\n", name1, (*one).prop[i].value);
			gotN1 = 1;//set that we go a full name
		}
	}
	for (j = 0; j < (*two).nprops; j++){//do the same as above for the second card
		if ((*two).prop[j].name == VCP_N && gotN2 == 0){
			name2 = malloc(sizeof(char*)*strlen((*two).prop[j].value)+1);
			assert(name2);
			strcpy(name2, (*two).prop[j].value);
			//printf("NAME IS: %s, copied from %s\n", name2, (*two).prop[j].value);
			gotN2 = 1;
		}
	}

	//malloc for first and last names of both cards
	firstName1 = malloc(sizeof(char*)*strlen(name1));
	lastName1 = malloc(sizeof(char*)*strlen(name1));
	firstName2 = malloc(sizeof(char*)*strlen(name2));
	lastName2 = malloc(sizeof(char*)*strlen(name2));

	assert(firstName1);
	assert(lastName1);
	assert(firstName2);
	assert(lastName2);



	//tokenize card names into first and last.
	token = NULL;
	token = strtok(name1, ";");
	if (token != NULL){
		strcpy(lastName1, token);
	}
	else{
		lastName1 = "";
	}
	token = strtok(NULL, ";");
	if (token != NULL){
		strcpy(firstName1, token);
	}
	else{
		firstName1 = "";
	}

	token = NULL;//reset token for next cards names
	
	token = strtok(name2, ";");
	if (token != NULL){
		strcpy(lastName2, token);
	}
	else{
		lastName2 = "";
	}
	token = strtok(NULL, ";");
	if (token != NULL){
		strcpy(firstName2, token);
	}
	else{
		firstName2 = "";
	}

	free(name1);
	free(name2);

	//printf("firstName1: %s, lastName1: %s\n", firstName1, lastName1);
	//printf("firstName2: %s, lastName2: %s\n", firstName2, lastName2);

	if ( strcasecmp(lastName1, lastName2) != 0 ){//if the last names dont match, then sort by this
		/*free(firstName1);
		free(firstName2);
		free(lastName1);
		free(lastName2);*/
		//printf("comp = %d\n", strcasecmp(lastName1, lastName2));
		return strcasecmp(lastName1, lastName2);
	}
	else if ( strcasecmp(lastName1, lastName2) == 0){//if last names the same, then sort by first name
		if (strcasecmp(firstName1, firstName2) != 0){
			//printf("comp = %d\n", strcasecmp(firstName1, firstName2));
			return strcasecmp(firstName1, firstName2);
		}
		else{//if both names are the same then return they are the same!
			free(firstName1);
			free(firstName2);
			free(lastName1);
			free(lastName2);
			return 0;
		}
	}

	return EXIT_SUCCESS;
}

/*The main vcftool where we handle which functions 
	the user would like to call
*/
int main(int argc, char * argv[]){

	VcFile * filep = malloc(sizeof(VcFile));
	char * which;
	VcStatus stat;
	stat.code = OK;
	stat.lineto = 0;
	stat.linefrom = 0;

	if ( strcmp(argv[1], "-info") == 0 ){
		if (argc > 2){
			if ((strcmp(argv[2], "-sort") == 0 || strcmp(argv[2], "-canon") == 0 
				|| strcmp(argv[2], "-select") == 0)){
				free(filep);
				return EXIT_FAILURE;
			}
		}
		//printf("calling info\n");
		stat = readVcFile(stdin, filep);
		if ( stat.code == OK){
			vcfInfo(stdout, filep);//make the output, standard out
		}
		else{
			fprintf(stderr, "Error with file\n");
			//free(filep);
			return EXIT_FAILURE;
		}
	}

	else if ( strcmp(argv[1], "-canon") == 0 ){
		if (argc > 2){
			if ((strcmp(argv[2], "-sort") == 0 || strcmp(argv[2], "-select") == 0 
				|| strcmp(argv[2], "-info") == 0)){
				free(filep);
				return EXIT_FAILURE;
			}
		}
		//printf("calling canonical\n");
		stat = readVcFile(stdin, filep);
		if ( stat.code == OK){
			stat.code = vcfCanon(filep);
			if (stat.code == OK){
				//printf("GOT TO WRITE FOR CANON\n");
				writeVcFile(stdout, filep);
			}
		}
		else{
			fprintf(stderr, "Error with file\n");
			//free(filep);
			return EXIT_FAILURE;
		}
	}

	else if ( strcmp(argv[1], "-select") == 0 ){
		if (argc > 2){
			if ((strcmp(argv[2], "-sort") == 0 || strcmp(argv[2], "-canon") == 0 
				|| strcmp(argv[2], "-info") == 0)){
				fprintf(stderr, "Error with arguements\n");
				free(filep);
				return EXIT_FAILURE;
			}
		}
		//printf("calling select\n");
		stat = readVcFile(stdin, filep);//read file from standard in, and create vcards
		if ( stat.code == OK){
			which = malloc(sizeof(char*)*strlen(argv[2])+1);//FIXTAG, need to free this
			assert(which);
			strcpy(which, argv[2]);//get the arguement for select
			if ( strstr(which, "p") || strstr(which, "u") || strstr(which, "g") ){
				stat.code = vcfSelect(filep, which);
				if (stat.code == OK){//FIXTAG, added 
					writeVcFile(stdout, filep);//FIXTAG
				}
			}
			else{
				fprintf(stderr, "Error with arguements in which\n" );
				free(filep);
				return EXIT_FAILURE;
			}
		}
		else{
			fprintf(stderr, "Error with file\n" );
			//free(filep);
			return EXIT_FAILURE;
		}
	}

	else if ( strcmp(argv[1], "-sort") == 0 ){
		if (argc > 2){
			if ((strcmp(argv[2], "-select") == 0 || strcmp(argv[2], "-canon") == 0 
				|| strcmp(argv[2], "-info") == 0)){
				fprintf(stderr, "Error with arguements\n");
				free(filep);
				return EXIT_FAILURE;
			}
		}
		//printf("calling sort\n");
		stat = readVcFile(stdin, filep);//read file from standard in, and create vcards
		if ( stat.code == OK){
			stat.code = vcfSort(filep);
			if (stat.code == OK){//FIXTAG, added this
				writeVcFile(stdout, filep);
			}
		}
		else{
			fprintf(stderr, "Error with file\n");
			//free(filep);
			return EXIT_FAILURE;
		}
	}
	else{//if the command was not typed properly or is unrecognized.
		fprintf(stderr, "ERROR: not a recognized command!\n");
		free(filep);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/* This function takes in a file pointer to vcards and a 
	string of chars that are designed to select cards with 
	certain properties only.
	Photo, url and geo are the properties we can select.*/
int vcfSelect( VcFile *const filep, const char *which ){

	int i = 0, j = 0, k = 0, wantP = 0, wantUrl = 0, wantGeo = 0;
	int gotP = 0, gotUrl = 0, gotGeo = 0;
	int cardsStored = 0, shift = 0;
	int cardsFreed = 0;
	char * p, * u, * g;

	if (filep->ncards == 0){
		fprintf(stderr, "ERROR: No cards to work with\n");
		return EXIT_FAILURE;
	}
	else{
		if ( (strlen(which) > 3) ){//if user put in more than 'pug', 3 chars, its an error
			fprintf(stderr, "ERROR: use 'p', 'u' and 'g' only and only one instance of each.\n");
			return EXIT_FAILURE;
		}

		if ( (p = strstr(which, "p")) ){//if we want cards with photos
			wantP += 1;//count instances of wanting a photo
		}
		if ( (u = strstr(which, "u")) ){//if we want urls
			wantUrl += 1;
		}
		if ( (g = strstr(which, "g")) ){//if we want geo
			wantGeo += 1;
		}
		if ( ((wantP == 0) && (wantUrl == 0) && (wantGeo == 0)) || 
			((wantP > 1) || (wantUrl > 1) || (wantGeo > 1)) ){
			//printf("%s: %d %d %d\n",which, wantP, wantUrl, wantGeo);
			fprintf(stderr, "ERROR: use 'p', 'u' and 'g' only and only one instance of each.\n");
			return EXIT_FAILURE;
		}

		//printf("Cards to loop through %d\n", filep->ncards);
		for(i = 0; i < filep->ncards; i++){//loop through number of cards
			//printf("Cards Freed %d, cards left %d\n", cardsFreed, filep->ncards);
			gotP = 0;
			gotUrl = 0;
			gotGeo = 0;

			for(j = 0; j < filep->cardp[i]->nprops; j++){//loop through properties
				//counting the photo, url, geo and uid tags
				if( (filep->cardp[i]->prop[j].name == VCP_PHOTO) && wantP == 1){
					//printf("GOT PHOTO: %s\n", filep->cardp[i]->prop[j].value);
					gotP = 1;
				}
				else if( (filep->cardp[i]->prop[j].name == VCP_URL) && wantUrl == 1){
					gotUrl = 1;
				}
				else if( (filep->cardp[i]->prop[j].name == VCP_GEO) && wantGeo == 1){
					gotGeo = 1;
				}
			}

			if ( ((wantP == 1 && gotP == 1) || (wantP == 0 && gotP == 0)) 
				&& ((wantUrl == 1 && gotUrl == 1) || (wantUrl == 0 && gotUrl == 0)) 
				&& ((wantGeo == 1 && gotGeo == 1) || (wantGeo == 0 && gotGeo == 0)) ){
				cardsStored += 1;
				//printf("Storing the card: %d\n", i);
				gotP = 0;
				gotUrl = 0;
				gotGeo = 0;
				//then store the card!
			}
			else{
				//printf("Freeing this card! at %d\n", i);
				//free all the properties of the card that we didnt select
				for(j = 0; j < filep->cardp[i]->nprops; j++){
					if ( filep->cardp[i]->prop[j].parval != NULL){
						free(filep->cardp[i]->prop[j].parval);//free the parval property
						filep->cardp[i]->prop[j].parval = NULL;
					}
					if ( filep->cardp[i]->prop[j].partype != NULL){
						free(filep->cardp[i]->prop[j].partype);
						filep->cardp[i]->prop[j].partype = NULL;
					}
					if ( filep->cardp[i]->prop[j].value != NULL){
						free(filep->cardp[i]->prop[j].value);
						filep->cardp[i]->prop[j].value = NULL;
					}
				}

				//free the pointer to that card
				//printf("stats: i = %d, file is null = %d, cards left: %d\n", 
					//i, filep->cardp[i] == NULL, filep->ncards);
				if ( filep->cardp[i] != NULL){
					//printf("GOT IN HERE TO FREE CARDP, freeing card: %d\n", i);
					free(filep->cardp[i]);//FIXTAG, causing segfault here with MANY CARD FILE
					filep->cardp[i] = NULL;//FIXTAG
					cardsFreed += 1;
					filep->ncards -= 1;
					shift = i;
				}
				//printf("just before shifting cards\n");
				for (k = shift; k < filep->ncards; k++){
					if (filep->cardp[k+1] != NULL){
						//printf("SHIFTING THE CARD %d to %d\n", k+1, k);
						filep->cardp[k] = filep->cardp[k+1];
					}
				}
				i -= 1;//decrement where we are looping at in the cards
				//filep->ncards -= 1;//we have 1 less card now
			}
		}
	}

	if (cardsStored == 0){
		fprintf(stderr, "No cards selected.");
		return EXIT_SUCCESS;
	}

	return EXIT_SUCCESS;
}

/* This function takes in the file pointer and output file
	It then collects the information about the vcards
	and prints that info to the user*/
int vcfInfo( FILE *const outfile, const VcFile *filep ){

	int photo = 0, url = 0, geo = 0, uid = 0;
	int foundP = 0, foundUrl = 0, foundGeo = 0, foundUid = 0;
	int sorted = 0, allSorted = 1, i = 0, j = 0;

	if (outfile != NULL){
		//printf("GOT %d cards\n", filep->ncards);
		for(i = 0; i < filep->ncards; i++){//loop through number of cards
			for(j = 0; j < filep->cardp[i]->nprops; j++){//loop through properties

				//card sort check
				if (i < filep->ncards-1 && filep->ncards > 1){//&& filep->cardp[i] != NULL
					//used to be reversed: cardcmp &filep->cardp[i+1], &filep->cardp[i]
					sorted = cardCmp(&filep->cardp[i+1], &filep->cardp[i]);
					if (sorted < 1){
						//printf("sorted = %d\n", sorted);
						allSorted = 0;//set it so we know the cards are unsorted.
					}
				}
				//counting the photo, url, geo and uid tags
				if( (filep->cardp[i]->prop[j].name == VCP_PHOTO) && foundP == 0){
					photo += 1;
					foundP = 1;
				}
				else if( (filep->cardp[i]->prop[j].name == VCP_URL) && foundUrl == 0){
					url += 1;
					foundUrl = 1;
				}
				else if( (filep->cardp[i]->prop[j].name == VCP_GEO) && foundGeo == 0){
					geo += 1;
					foundGeo = 1;
				}
				else if( (filep->cardp[i]->prop[j].name == VCP_UID) && foundUid == 0){
					uid += 1;
					foundUid = 1;
				}
			}
			foundP = 0;//reset the tracking booleans
			foundUrl = 0;
			foundGeo = 0;
			foundUid = 0;
		}
	}
	else{
		//printf("GOT NULL FILE\n");
		return EXIT_FAILURE;
	}
	//printf("Sorted at the end: %d\n", sorted);

	if (allSorted == 1){
		fprintf(outfile, "%d cards (sorted)\n", filep->ncards);
	}
	else{
		fprintf(outfile, "%d cards (not sorted)\n", filep->ncards);
	}

	fprintf(outfile, "%d with photos\n", photo);
	fprintf(outfile, "%d with URLs\n", url);
	fprintf(outfile, "%d with geographical coordinates\n", geo);
	fprintf(outfile, "%d in canonical form\n", uid);

	return EXIT_SUCCESS;
}

/* This function takes a file pointer and checks for canonicalization
	of particular properties Name, ADR, TEL and GEO. It sees in its helper
	vcfCanProp, whether it can modify the property to canon or not 
	and writes possible changes to the card*/
int vcfCanon( VcFile *const filep ){

	int i = 0, j = 0;
	int result = 0;
	int nameIsCanon = 1, adrIsCanon = 1, telIsCanon = 1, geoIsCanon = 1;
	int nameNum = 0, adrNum = 0, telNum = 0, geoNum = 0;
	int gotUID = 0, uidPos = 0;
	char * uidValue;

	for(i = 0; i < filep->ncards; i++){//loop through number of cards
		nameNum = 0;
		adrNum = 0;
		telNum = 0;
		geoNum = 0;
		for(j = 0; j < filep->cardp[i]->nprops; j++){//loop through properties
			gotUID = 0;
			//send property to vcfcanprop
			result = vcfCanProp(&filep->cardp[i]->prop[j]);
			if (result == 1){//property is already in canon form
				//check for a uid value and check to either update uid or write a new uid value
				if (filep->cardp[i]->prop[j].name == VCP_N){
					nameNum += 1;
				}
				else if (filep->cardp[i]->prop[j].name == VCP_ADR){
					adrNum += 1;
				}
				else if (filep->cardp[i]->prop[j].name == VCP_TEL){
					telNum += 1;
				}
				else if (filep->cardp[i]->prop[j].name == VCP_GEO){
					geoNum += 1;
				}
			}
			else if (result == 2){//we succeeded in making it canonical
				if (filep->cardp[i]->prop[j].name == VCP_N){
					nameNum += 1;
				}
				else if (filep->cardp[i]->prop[j].name == VCP_ADR){
					adrNum += 1;
				}
				else if (filep->cardp[i]->prop[j].name == VCP_TEL){
					telNum += 1;
				}
				else if (filep->cardp[i]->prop[j].name == VCP_GEO){
					geoNum += 1;
				}
			}
			else if (result == 3){//canononical failed
				if (filep->cardp[i]->prop[j].name == VCP_N){
					nameIsCanon = 0;
				}
				else if (filep->cardp[i]->prop[j].name == VCP_ADR){
					adrIsCanon = 0;
				}
				else if (filep->cardp[i]->prop[j].name == VCP_TEL){
					telIsCanon = 0;
				}
				else if (filep->cardp[i]->prop[j].name == VCP_GEO){
					geoIsCanon = 0;
				}
			}
			else if (result == 0){//canon does not apply.
				//do nothing about it
			}
			if (filep->cardp[i]->prop[j].name == VCP_UID){
				//printf("UID PROPERTY \n");
				gotUID = 1;
				uidPos = j;
			}
		}

		//modify uid of card
		uidValue = NULL;
		uidValue = malloc(sizeof(char* )*20);
		assert(uidValue);
		strcpy(uidValue, "@");
		if (nameIsCanon == 1){//based on if properties are canon or not
			if (nameNum > 0){
				strcat(uidValue, "N");//write this value
			}
			else{
				strcat(uidValue, "-");
			}
		}
		else{
			strcat(uidValue, "*");
		}

		if (adrIsCanon == 1){//if adr is canon
			if (adrNum > 0){
				strcat(uidValue, "A");
			}
			else{
				strcat(uidValue, "-");
			}
		}
		else{
			strcat(uidValue, "*");
		}

		if (telIsCanon == 1){//if tel is canon
			if (telNum > 0){
				strcat(uidValue, "T");
			}
			else{
				strcat(uidValue, "-");
			}
		}
		else{
			strcat(uidValue, "*");
		}

		if (geoIsCanon == 1){//if geo is canon
			if (nameNum > 0){
				strcat(uidValue, "G");
			}
			else{
				strcat(uidValue, "-");
			}
		}
		else{
			strcat(uidValue, "*");
		}
		strcat(uidValue, "@");//write end of uidValue
		strcat(uidValue, "\0");

		if (gotUID == 1){//if there was already a uid value
			strcat(uidValue, ";");
			strcat(uidValue, filep->cardp[i]->prop[uidPos].value);
			free(filep->cardp[i]->prop[uidPos].value);
			filep->cardp[i]->prop[uidPos].value = NULL;
			filep->cardp[i]->prop[uidPos].value = uidValue;//update the uid
		}
		else{
			//fprintf(filep->cardp[i], "UID:%s\n", uidValue);
			filep->cardp[i] = realloc ( filep->cardp[i], sizeof (Vcard) + 
				sizeof(VcProp)*(filep->cardp[i]->nprops + 1));
			filep->cardp[i]->nprops += 1;//add one property
			filep->cardp[i]->prop[filep->cardp[i]->nprops-1].name = VCP_UID;
			filep->cardp[i]->prop[filep->cardp[i]->nprops-1].value = malloc(sizeof(char*)
				*strlen(uidValue)+1);
			assert(filep->cardp[i]->prop[filep->cardp[i]->nprops-1].value);
			filep->cardp[i]->prop[filep->cardp[i]->nprops-1].value = uidValue;
		}
		
	}
	//writeVcFile(stdout, filep);

	return EXIT_SUCCESS;
}


/* Helper function to vcfCanon. This function takes a property
	and checks if it is in canonical form or if it can be modified 
	into canonical form. It returns the status of which operation happened*/
int vcfCanProp( VcProp *const propp ){

	int semiColonCount = 0, i = 0, k = 0, m = 0;
	int changedProp = 0, origRegLen = 0;
	char * insert, * token;
	char * region, * country;
	char * tempReg, * tempCountry;

	if (propp->name != VCP_N && propp->name != VCP_ADR && propp->name != VCP_TEL 
		&& propp->name != VCP_GEO){
		//this property does not apply to canonicalization
		return 0;
	}
	else{
		if (propp->name == VCP_N){//IF WE GOT A NAME PROPERTY
			changedProp = 0;
			semiColonCount = 0;
			//printf("name: %s\n", propp->value);
			if (strlen(propp->value) < 2){
				propp->value = "(none);(none)";
				semiColonCount = 1;
			}
			else{
				for (i = 0; i < strlen(propp->value); i++){
					if (propp->value[i] == ';' && (i != 0) ) {//if we get a semi colon
						semiColonCount += 1;//count it
						if ( i+1 < strlen(propp->value) ){//if we have a next element
							//check that its uppercase
							if ( !isupper(propp->value[i+1]) && (propp->value[i+1] != ';' )){
								changedProp = 1;//note we adjusted property
								propp->value[i+1] = toupper(propp->value[i+1]);//change case
								//printf("changed to: %c\n", propp->value[i+1]);
							}
							//if first name needs to be none
							if ( propp->value[i+1] == ';' && (semiColonCount < 2)){
								changedProp = 1;
								insert = malloc(sizeof(char*)*strlen(propp->value)+6);
								assert(insert);
								strncpy(insert, propp->value, i+1);
								insert[i+1] = '\0';//add null terminator
								strcat(insert, "(none);");
								for (int j = i+1; j < strlen(propp->value); j++){
									insert[i+j+2] = propp->value[j];
								}
								strcpy(propp->value, insert);
								free(insert);
								insert = NULL;
							}
						}
						else if (semiColonCount == 1){
							//filep->cardp[i] = realloc ( filep->cardp[i], sizeof (Vcard) + 
							//sizeof (VcProp) * (filep->cardp[i]->nprops + 1));
							//propp->value = realloc( propp->value, sizeof(VcProp) * 20);
							insert = malloc(sizeof(char*)*strlen(propp->value)+10);
							assert(insert);
							strcpy(insert, propp->value);
							strcat(insert, "(none)");
							strcpy(propp->value, insert);
						}
					}
					if (propp->value[i] == ';' && (i == 0) ){//if last name needs to be set to none
						changedProp = 1;
						semiColonCount += 1;
						insert = malloc(sizeof(char*)*strlen(propp->value)+6);
						assert(insert);
						strcat(insert, "(none)");
						strcat(insert, propp->value);
						strcpy(propp->value, insert);
						free(insert);
						insert = NULL;
					}
					else if ( i == 0 ){//check if first name is capitalized
						if ( !isupper(propp->value[i]) ){//check that its uppercase
							changedProp = 1;
							propp->value[i] = toupper(propp->value[i]);//if it isn't change it to uppercase
							//printf("firstname to: %c\n", propp->value[i]);
						}
					}
				}
				if (semiColonCount >= 1 && semiColonCount < 4){
					/*strcpy(insert, propp->value);
					free(propp->value);
					propp->value = NULL;
					propp->value = malloc(sizeof(char*)*strlen(insert)+6);
					strcpy(propp->value, insert);*/
					while (semiColonCount < 4 ){//while we don't have enough semicolons
						strcat(propp->value, ";");//add them
						semiColonCount += 1;
					}
				}
			}
			
			//printf("after: %s\n", propp->value);
			if (semiColonCount < 1){
				//data is too short, can't be recognized so it fails
				//printf("3!\n");
				return 3;
			}
			if (changedProp == 1){//we modified the property, so return that canonicalization succeeded
				//printf("2!\n");
				return 2;
			}
			else{//it was already in canonical form
				//printf("1!\n");
				return 1;
			}
		}

		else if (propp->name == VCP_ADR){//FOR ADR PROPERTIES
			changedProp = 0;
			semiColonCount = 0;
			int changedRegion = 0, countryForeign = 1;
			region = malloc(sizeof(char*)*strlen(propp->value)+1);
			country = malloc(sizeof(char*)*strlen(propp->value)+1);
			assert(region);
			assert(country);
			
			//printf("ADR: %s\n", propp->value);
			
			k = 0;
			m = 0;
			for (i = 0; i < strlen(propp->value); i++){
				if (propp->value[i] == ';') {//if we get a semi colon
					semiColonCount += 1;//count it
				}
				if (semiColonCount == 4){//start grabbing the region
					region[k] = propp->value[i+1];
					k += 1;
				}
				if (semiColonCount == 6){//grab the country 
					country[m] = propp->value[i+1];
					m += 1;
				}
			}
			origRegLen = strlen(region);
			region[k-1] = '\0';//remove the semi colon
			country[m-1] = '\0';//remove space at end
			//printf("region: %s, country: %s\n", region, country);

			if (strlen(country) > 2){//if the country is not in code format
				changedProp = 1;
				//convert country to code
				tempCountry = malloc(sizeof(char*)*3);
				assert(tempCountry);
				if ( strlen(tempCountry = countryToCode(country)) == 2){
					free(country);
					country = NULL;
					country = tempCountry;//set new country value
				}
				else{
					//it failed finding country
					return 3;
				}
			}

			if ( (strcmp(country, "CA") == 0) || (strcmp(country, "US") == 0) ){
				countryForeign = 0;
			}

			//if region is not in code format and country is Canada or USA.
			if ( (strlen(region) > 2) && ( (strcmp(country, "CA") == 0) 
				|| (strcmp(country, "US") == 0) ) ){

				changedProp = 1;
				changedRegion = 1;
				tempReg = malloc(sizeof(char*)*3);
				assert(tempReg);
				if ( strlen(tempReg = provinceToCode(region)) == 2){//convert to province if we can
					free(region);
					region = NULL;
					region = tempReg;
				}
				else if ( strlen(tempReg = stateToCode(region)) == 2 ){//convert to state if we can
					free(region);
					region = NULL;
					region = tempReg;
				}
				else{
					//it failed finding region
					return 3;
				}
			}
			
			//printf("Region code: %s, Country code: %s\n", region, country);
			//printf("Semi colons: %d\n", semiColonCount);

			if (semiColonCount < 6 || (strlen(region) < 2) || strlen(country) < 2){
				//data is too short, can't be recognized so it fails
				return 3;
			}
			if (changedProp == 1){//we modified the property, so return that canonicalization succeeded
				k = 0;
				m = 0;
				insert = malloc(sizeof(char*)*strlen(propp->value)+1);
				assert(insert);
				semiColonCount = 0;

				for (i = 0; i < strlen(propp->value); i++){

					if (propp->value[i] == ';') {//if we get a semi colon
						semiColonCount += 1;//count it
					}
					if ( (semiColonCount == 4) && (k < 2) ){
						propp->value[i+1] = region[k];
						k += 1;
					}
					if ( (semiColonCount == 4) && (k == 2) ){
						if (changedRegion == 1){
							propp->value[i+2] = ';';
							semiColonCount += 1;
						}
						k += 1;
					}
					if ( (semiColonCount == 5) && (k > 3) && countryForeign == 0){
						int offset = i - origRegLen + 3;
						insert[offset] = propp->value[i];
						k += 1;
					}
					if ( (semiColonCount == 5) && (k == 3) ){
						if (changedRegion == 0 && countryForeign == 1){
							strncpy(insert, propp->value, i+k-1);
							insert[i+k-1] = '\0';
							//printf("insert = %s\n", insert);
							strcat(insert, country);
						}
						else{
							strncpy(insert, propp->value, i+k);
						}
						k += 1;
						i += origRegLen-1;
					}

					if (semiColonCount == 6 && m < 1){ 
						strcat(insert, ";");
						strcat(insert, country);
						m += 1;
					}
				}

				free(propp->value);//free old value
				propp->value = NULL;//set null
				propp->value = insert;//set value to new value
				//printf("2!\n");
				return 2;
			}
			else{//it was already in canonical form
				//printf("1!\n");
				return 1;
			}

		}

		else if (propp->name == VCP_TEL){//FOR TELEPHONE PROPERTIES
			changedProp = 0;
			semiColonCount = 0;
			int numDigits = 0, numArea = 0, numBeforeArea = 0;
			int gotArea = 0, gotLongDist = 0, foundExt = 0;

			insert = malloc(sizeof(char*)*strlen(propp->value)+10);
			assert(insert);
			strcpy(insert, propp->value);

			for (i = 0; i < strlen(insert); i++){

				if (insert[i] != '+' && i == 0){
					strcpy(insert, "+1 ");
					strcat(insert, propp->value);
					changedProp = 1;
				}
				else if (i == 0){
					gotLongDist = 1;
				}

				if (insert[i] == '('){//if we found the start of an area code
					numBeforeArea = numDigits;//numbers before we found area code
					if (i+5 <= strlen(insert)){
						for (k = i; k < i + 5; k++){
							//printf("char is: %c\n", insert[k]);
							if (isdigit(insert[k])){
								numArea += 1;
							}
							if (insert[k] == ')' && numArea == 3){
								gotArea = 1;
							}
						}
					}
				}
				if (isdigit(insert[i]) ){
					numDigits += 1;
				}
				if (insert[i] == 'E'){
					foundExt = 1;
				}
			}
			if (numBeforeArea == 1 && gotArea == 1){//if we had a num before area code and an area code
				gotLongDist = 1;
			}
			//printf("total digits: %d, area code total: %d, got Area code = %d\n\n", 
				//numDigits, numArea, gotArea);

			if (gotLongDist == 0 && numDigits == 11){
				char * modedTel = malloc(sizeof(char*)*strlen(insert)+1);
				changedProp = 1;
				strncpy(modedTel, insert, 3);
				strcat(modedTel, "(");

				strncat(modedTel, insert+3, 3);
				strcat(modedTel, ") ");
				strcat(modedTel, insert+7);
				free(insert);
				insert = NULL;
				insert = modedTel;
			}

			if (foundExt == 1 && numDigits > 11){
				char * modedTel = malloc(sizeof(char*)*strlen(insert)+1);
				changedProp = 1;
				strncpy(modedTel, insert, 17);
				strcat(modedTel, "|");

				strcat(modedTel, insert+23);


				free(insert);
				insert = NULL;
				insert = modedTel;
			}


			if (numDigits < 7){//error
				return 3;
			}
			if (changedProp == 1 && gotLongDist == 0){
				free(propp->value);
				propp->value = NULL;
				propp->value = insert;
				return 2;
			}
			else if (changedProp == 1){
				free(propp->value);
				propp->value = NULL;
				propp->value = insert;
				return 2;
			}
			else{
				free(insert);
				return 1;
			}
			
		}

		else if (propp->name == VCP_GEO){//for latitude and longitude geographic properties
			//printf("geo: %s\n", propp->value);
			changedProp = 0;
			semiColonCount = 0;

			char * lat , * lon;
			int beforeDecimal = 0, latDec = 0, lonDec = 0;
			int toDecimalLen = 0, latNeg = 0, lonNeg = 0;
			int toDelim = 0, singleLen = 0;
			int gotDecimal = 0;
			char * latBeforeD, * lonBeforeD;

			token = NULL;
			insert = malloc(sizeof(char*)*strlen(propp->value)+1);
			lat = malloc(sizeof(char*)*strlen(propp->value)+1);
			lon = malloc(sizeof(char*)*strlen(propp->value)+1);
			latBeforeD = malloc(sizeof(char*)*strlen(propp->value)+1);
			lonBeforeD = malloc(sizeof(char*)*strlen(propp->value)+1);
			assert(insert);
			assert(lat);
			assert(lon);
			assert(latBeforeD);
			assert(lonBeforeD);
			strcpy(insert, propp->value);

			if ( (token = strtok(insert, ";")) != NULL ){

				//TOKENIZE LATITUDE and LONGITUDE HERE

				strcpy(lat, token);
				singleLen = strlen(lat);
				token = strtok(NULL, "\n");
				if (token == NULL){//if we didn't have a semi colon, we have to delimit another way
					changedProp = 1;
					for (i = 0; i < strlen(lat); i++){
						if (lat[i] == ' ' || lat[i] == ','){//break at possible delimeter
							toDelim = i-1;
							singleLen = singleLen - toDelim;
							for (k = 0; k < singleLen; k++){
								lon[k] = lat[i+k+1];//copy out longitude from the end of lat
							}
							lat[i] = '\0';//set null terminators
							lon[k] = '\0';
							break;//break we are done here.
						}
						
					}
				}
				else{//if we got the delimiter ';' then we can just copy 
					strcpy(lon, token);
				}

				//WORK WITH LATITUDE
				//printf("strlen of lat: %d\n", strlen(lat));
				beforeDecimal = 0;
				toDecimalLen = 0;
				for (i = 0; i < strlen(lat); i++){
					toDecimalLen += 1;
					if (lat[i] == '.'){//break at decimal point
						gotDecimal = 1;
						break;
					}
					else if (isdigit(lat[i])){//if its a digit
						latBeforeD[beforeDecimal] = lat[i];
						beforeDecimal += 1;//then count it as a digit before the decimal
					}
					else if (lat[i] == '-'){
						latNeg = 1;
					}
				}
				//printf("1 toDecimalLen = %d, strlen(lat) = %d\n", toDecimalLen, strlen(lat));
				if (toDecimalLen == strlen(lat) && gotDecimal == 0){
					strcat(lat, ".");//add a decimal manually.
				}
				//printf("2 toDecimalLen = %d, strlen(lat) = %d\n", toDecimalLen, strlen(lat));
				if (strlen(latBeforeD) <= 1){
					return 3;
				}
				latDec = atoi(latBeforeD);
				//printf("1) toDecimalLen= %d, latBeforeD = %s\n", toDecimalLen, latBeforeD);

				//if more than 2 digits or the latitude is out of range, we have an error.
				if (beforeDecimal > 2 || ( latDec > 90)){
					//printf("we can't have more than -90 to 90 in latitude.\n");
					return 3;//we can't have more than -90 to 90 in latitude.
				}
				if (latDec == 90){
					for(i = beforeDecimal+2; i < strlen(lat); i++){
						//printf("char %c\n", lat[i]);
						if (lat[i] != '0'){
							//printf("ABOVE 90!\n");
							return 3; //we are actually above the degree requirement.
						}
					}
				}
				if (strlen(lat) < 9 ){//if less than 9 digits, then
					changedProp = 1;
					if (latNeg == 0){
						if (strlen(lat) == 2){
							//printf("in here2!\n");
							for (i = strlen(lat); i < 8; i++){
								strcat(lat, "0");
							}
						}
						else{
							for (i = strlen(lat); i < 9; i++){
								strcat(lat, "0");
							}
						}
					}
					else{
						//printf("strlen(lat) %d\n", strlen(lat));
						if (strlen(lat) == 3){
							//printf("in here1!\n");
							for (i = strlen(lat); i < 9; i++){
								strcat(lat, "0");
							}
						}
						else{
							for (i = strlen(lat); i < 10; i++){
								strcat(lat, "0");
							}
						}
					}
				}

				//WORK WITH LONGITUDE DOWN HERE;
				//printf("strlen of long: %d\n", strlen(lon));
				beforeDecimal = 0;
				toDecimalLen = 0;
				gotDecimal = 0;
				for (i = 0; i < strlen(lon); i++){
					toDecimalLen += 1;
					if (lon[i] == '.'){//break at decimal point
						gotDecimal = 1;
						break;
					}
					else if (isdigit(lon[i])){//if its a digit
						lonBeforeD[beforeDecimal] = lon[i];
						beforeDecimal += 1;//then count it as a digit before the decimal
					}
					else if (lon[i] == '-'){
						lonNeg = 1;
					}
				}
				//printf("toDecimalLen = %d, strlen(lat) = %d\n", toDecimalLen, strlen(lon));
				if ( toDecimalLen == strlen(lon) && gotDecimal == 0 ){//check that we go a decimal
					strcat(lon, ".");//add decimal manually.
				}
				lonDec = atoi(lonBeforeD);
				//printf("longBeforeD = %s\n", lonBeforeD);
				if (beforeDecimal > 3 || (lonDec > 180 )){//if more than 3 digits, then theres an error.
					//printf("we can't have more than -180 to 180 in longitude.\n");
					return 3;//we can't have more than -90 to 90 in latitude.
				}
				if (lonDec == 180){
					for(i = beforeDecimal+2; i < strlen(lon); i++){
						if (lon[i] != '0'){
							//printf("ABOVE 180!\n");
							return 3; //we are actually above the degree requirement.
						}
					}
				}
				if (strlen(lon) < 8){
					changedProp = 1;
					if (lonNeg == 0){//if its negative, then cat this many 0's
						if (strlen(lon) == 2){//if its a small digit number
							for (i = strlen(lon); i < 8; i++){
								strcat(lon, "0");
							}
						}
						else{
							for (i = strlen(lon); i < 10; i++){
								strcat(lon, "0");
							}
						}
					}
					else{//if its not negative
						if (strlen(lon) == 3){//small digit number
							for (i = strlen(lon); i < 9; i++){
								strcat(lon, "0");
							}
						}
						else{
							for (i = strlen(lon); i < 10; i++){
								strcat(lon, "0");
							}
						}
					}
					
				}
				//printf("lat: %s\nlong: %s\n", lat, lon);

				if (changedProp == 1){//we successfully changed into canonical form

					free(propp->value);
					propp->value = NULL;
					propp->value = malloc(sizeof(char*)* (strlen(lat) + strlen(lon) + 2));
					assert(propp->value);

					strcat(propp->value, lat);
					strcat(propp->value, ";");
					strcat(propp->value, lon);

					/*free(insert);
					free(lat);
					free(lon);
					free(latBeforeD);
					free(lonBeforeD);*/

					//printf("NEW PROPP VALUE= %s\n\n\n", propp->value);
					return 2;
				}
				else{//already in canonical form!
					//printf("1!\n\n");
					return 1;
				}
			}
			else{//There isn't enough to look at
				//printf("3!\n");
				return 3; 
			}
		}

	}

	return EXIT_SUCCESS;
}

/* This function takes in the file pointer and sorts the cards in it.
	This function calls qsort to sort the cards.*/
int vcfSort( VcFile *const filep ){

	qsort(&filep->cardp[0], filep->ncards, sizeof(Vcard*), cardCmp);

	return EXIT_SUCCESS;
}
