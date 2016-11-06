#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "my402transac.h"

#include "cs402.h"

#include "my402list.h"

#include <errno.h>

#include<sys/stat.h>


My402Transac *parseline(char *line);
void printlist(My402List *lp);
My402ListElem *findplace(My402List *lp, My402Transac *tp);
char *formatAmount(int cents);
int is_dir(char *path);

int main(int argc, char *argv[]) {

    errno = 0;

    //empty, just an anchor
    My402List list;
    memset(&list, 0, sizeof(My402List));
    (void)My402ListInit(&list);


    char buf[1026];
    buf[0] = '\0';//initialization
    buf[sizeof(buf)-1] = '\0';
    
    if(argc < 2 || argc > 3 || strcmp(argv[1], "sort") != 0) {

	fprintf(stderr, "malformed command\n");
	exit(1);
    }

    //read from stdin
    if(argc == 2) {


	My402Transac *tp;
	
	if ((fgets(buf, sizeof(buf), stdin)) != NULL) {
	    fprintf(stderr, "ERROR: Empty input from stdin.\n");
	    exit(1);	    
	}

	    do {

	    if(buf[sizeof(buf)-1] != '\0' && buf[sizeof(buf)-1] != '\n') {

		fprintf(stderr, "ERROR: line too long.");
		exit(1);
	    }
	    tp = parseline(buf);

	    //insert the object to the list
	    if(My402ListEmpty(&list)) {
		My402ListAppend(&list, tp);
	    }

	    else {
		My402ListElem * bp = findplace(&list, tp);
		My402ListInsertBefore(&list, tp, bp); 
	    }

	} while((fgets(buf, sizeof(buf), stdin)) != NULL);



	printlist(&list);

	free(tp->descrip);
	free(tp);	

    }

    //open file from command line
    else {

	FILE *fp;

	if(is_dir(argv[2])) {
	    fprintf(stderr, "input file %s is a directory\n", argv[2]);
	    exit(1);

	}


	//open failed
	if((fp = fopen(argv[2], "r")) == NULL) {
	    
	    if(errno == 2) {
	        fprintf(stderr, "input file %s does not exist\n", argv[2]);
	    }
	    else if(errno == 13) {
	        fprintf(stderr, "input file %s can not be opened - access denies\n", argv[2]);
	    }
	    exit(1);
	}

	//open file successfully
	else {
	    My402Transac *tp;

	    if ((fgets(buf, sizeof(buf), fp)) == NULL) {
	        fprintf(stderr, "Error: Empty input.\n");
	        exit(1);
	    }
	    do {

	        if(buf[sizeof(buf)-1] != '\0' && buf[sizeof(buf)-1] != '\n') {
		    fprintf(stderr, "ERROR: line too long.");
		    exit(1);
	        }

		
		tp = parseline(buf);

		//insert the object to the list
		if(My402ListEmpty(&list)) {
		    My402ListAppend(&list, tp);
		}
		else {
		    My402ListElem * bp = findplace(&list, tp);
		    My402ListInsertBefore(&list, tp, bp); 
		}
	    } while((fgets(buf, sizeof(buf), fp)) != NULL);


	    fclose(fp);
            
	    printlist(&list);

	    free(tp->descrip);
	    free(tp);
	}
    }
    return 0;
}


//parse line into a My402Transac, quit if parse failed
My402Transac *parseline(char *line) {

    char len[1024];
    strncpy(len, line, sizeof(len));
    if(len[1023] != '\0' && len[1023] != '\n') {

	fprintf(stderr, "ERROR: line is too long.\n");
	exit(1);
    }

    char add;
    int itime;
    //float amount;
    char amtl[8];
    amtl[7] = '\0';
    char amtr[3];
    amtr[2] = '\0';
    int dsize = 1026;
    char *descrip;

    //parse add
    char *startp = line;
    char *tabp = strchr(startp, '\t');

    if(tabp != NULL) {
        *tabp++ = '\0';
    }
    else {
	fprintf(stderr, "input file is not in the right format.\n");
	exit(1);
    }
    add = startp[0];
    if(add != '+' && add != '-') {
	fprintf(stderr, "ERROR: Transaction type (+-).\n");
	exit(1);
    }


    //parse time
    startp = tabp;
    tabp = strchr(startp, '\t');

    if(tabp != NULL) {
        *tabp++ = '\0';
    }
    else {
	fprintf(stderr, "ERROR: Missing Timestamp.\n");
	exit(1);
    }
    itime = atoi(startp);

	//convert timestamp to Date string
	time_t ttime = (time_t)itime;

	//current time
	time_t ltime;
	time(&ltime);

	if(ttime > ltime) {
	    fprintf(stderr, "Error: Transaction time.\n");
	    exit(1);
	}
	//printf("the time is %s", ctime(&ltime));


    //parse amount
    startp = tabp;
    tabp = strchr(startp, '\t');

    if(tabp != NULL) {
        *tabp++ = '\0';
    }
    else {
	fprintf(stderr, "ERROR: missing amount.\n");
	exit(1);
    }

    strncpy(amtl, startp, sizeof(amtl));

    char *pointp = strchr(startp, '.');
    if(pointp != NULL) {
        *pointp++ = '\0';
    }
    else {
	fprintf(stderr, "ERROR: Transaction amount(period).\n");
	exit(1);
    }
    
    strncpy(amtr, pointp, sizeof(amtr));


    //parse descrip
    startp = tabp;
    tabp = strchr(startp, '\t');

    if(tabp != NULL) {
	fprintf(stderr, "ERROR: too many fields.\n");
	exit(1);
    }


    if((descrip = malloc(dsize * sizeof(char))) == NULL) {
	fprintf(stderr, "ERROR: parseline: Malloc failed.\n");
	exit(1);		
    }
    descrip[dsize-1] = '\0';

    strncpy(descrip, startp, dsize);

    //descrip too long
    if(descrip[dsize-1] != '\0' && descrip[dsize-1] != '\n') {
	fprintf(stderr, "ERROR: parseline: the descrip is too long.\n");
	exit(1);	
    }


    if(amtr[0] == '\0' || amtr[1] == '\0' ||amtr[2] != '\0' ) {
	fprintf(stderr, "Error: Transaction amount(digits after period)\n");
	exit(1);	
    }


    int cent = 100*(int)atof(amtl) + atoi(amtr);
    if(cent >= 10000000 || cent < 0) {
	fprintf(stderr, "input number is too large!!!\n");
	exit(1);
    }



    My402Transac *tp;
    if((tp = malloc(sizeof(My402Transac))) == NULL) {
	fprintf(stderr, "ERROR: parseline: Malloc My402Transac failed\n");
	exit(1);
    }

    tp->add = add;
    tp->time = itime;
    tp->amount = cent;
    tp->descrip = descrip;

    return tp;
}

void printlist(My402List *lp) {

    printf("+-----------------+--------------------------+----------------+----------------+\n");
    printf("|       Date      | Description              |         Amount |        Balance |\n");
    printf("+-----------------+--------------------------+----------------+----------------+\n");

    My402ListElem *ep = NULL;

    int balance = 0;

    for(ep = My402ListFirst(lp); ep != NULL; ep = My402ListNext(lp, ep)) {

	My402Transac *tp = ep->obj;

	//Date field spans 3 through 17
	//len = 15

	//convert timestamp to Date string
	time_t time = (time_t)tp->time;


	//copy of the global variable returned by ctime()
	char date[16];
        char buf[26];
	strncpy(buf, ctime(&time), sizeof(buf));
	//last 2 characters of buf is '\n', '\0' ??
	int i;
	for(i = 0; i < 11; i++) {
	    date[i] = buf[i];
	}
	date[11] = buf[20];
	date[12] = buf[21];
	date[13] = buf[22];
	date[14] = buf[23];
	date[15] = '\0';

	//description field spans 21 through 44
	//len = 24
	char descrip[25];
	int l = 0;
	for(l = 0; l < 24; l++) {
	    descrip[l] = ' ';
	}
	strncpy(descrip, tp->descrip, sizeof(descrip));

	//after copy, a sequence of \0
	for(l = 0; l < 24; l++) {
	    //why second to last is \n???
	    if(descrip[l] == '\0' || descrip[l] == '\n')
	        descrip[l] = ' ';
	}
	descrip[24] = '\0';


	//The Amount field spans 48 through 61
	//len: 14
	char *famount;
	int amount = tp->amount;
	//
	if(amount >= 10000000 || amount < 0) {
	    if((famount = malloc(13*sizeof(char)))==NULL) {
		fprintf(stderr, "famount: malloc failed\n");
		exit(1);
	    }
	    famount = "?,???,???.??\0";
	}
	else {
	    famount = formatAmount(amount);
	}
	
	//printf("printlist: the format amount is: %s\n", famount);

	char *samount;
	if((samount= malloc(15*sizeof(char))) == NULL) {
		fprintf(stderr, "samount: malloc failed\n");
		exit(1);
	}
	//could use better string function?
	//cpy has to be in front of ()...interesting
	int j = 0;
	for(j = 0; j < 13; j++) {
            samount[j+1] = famount[j];
	}
	//samount[14] = '\0';	
        char add = tp->add;
	if(add == '+') {
	    samount[0] = ' ';
	    samount[13] = ' ';
	}
	else {
	    samount[0] = '(';
	    samount[13] = ')';	    
	}
	//printf("printlist: the samount is: %s\n", samount);


	//the Balance field spans 65 through 78
	//len: 14

	if(tp->add == '+') {
	    balance = balance + amount;
	}
	else {
	    balance = balance - amount;
	}

	char* fbalance;
	if(balance >= 10000000 || balance <= -10000000) {
	    if((fbalance = malloc(13*sizeof(char)))==NULL) {
		fprintf(stderr, "fbalance: malloc failed\n");
		exit(1);
	    }
	    famount = "?,???,???.??\0";
	}

	else {
	    fbalance = formatAmount(balance);
	}
	
	//printf("printlist: the format balance is: %s\n", fbalance);
	char *sbalance;
	if((sbalance= malloc(15*sizeof(char))) == NULL) {
		fprintf(stderr, "sbalance: malloc failed\n");
		exit(1);
	}
	//could use better string function?
	//cpy has to be in front of ()...interesting
	int k = 0;
	for(k = 0; k < 13; k++) {
            sbalance[k+1] = fbalance[k];
	}

	//sbalance[14] = '\0';	
	if(balance >= 0) {
	    sbalance[0] = ' ';
	    sbalance[13] = ' ';
	}
	else {
	    sbalance[0] = '(';
	    sbalance[13] = ')';	    
	}
	//printf("printlist: the sbalance is: %s\n", sbalance);

	printf("| %s | %s | %s | %s |\n", date, descrip, samount, sbalance);
        free(famount);
        free(samount);
        free(fbalance);
        free(sbalance);
    }

    printf("+-----------------+--------------------------+----------------+----------------+\n");
}


//PRE: the list is not empty
//the list is already sorted from small to big
//return the pointer to the bigger element 
My402ListElem *findplace(My402List *lp, My402Transac *tp) {

    int tpTime = tp->time;

    My402ListElem *ep = My402ListFirst(lp);
    My402Transac *ltp = ep->obj;
    int ltpTime = ltp->time;

    while(tpTime > ltpTime) {
	//move the element pointer, get its time
	ep = My402ListNext(lp, ep);    
	if(ep == NULL) {//if ep is the anchor
	    return &(lp->anchor);
	}
	ltp = ep->obj;

	ltpTime = ltp->time;
    }

    if(tpTime == ltpTime) {
	fprintf(stderr, "timestamps are identical!\n");
	exit(1);
    }
    return ep; 
}

//PRE: cents < 10000000
char *formatAmount(int cents) {
    if(cents < 0) {
	cents = -cents;
    }
    char *res;
    if((res = malloc(13*sizeof(char))) == NULL) {
	fprintf(stderr, "formatAmount: malloc failed.\n");
	exit(1);
    }

    //res = "        0.00";
    int i = 0;
    for(i = 0; i < 8; i++) {
	res[i] = ' ';
    }
    res[8] = '0';
    res[9] = '.';
    res[10] = '0';
    res[11] = '0';
    res[12] = '\0';

    int index = 11;

    while(cents != 0) {
	
	int last = cents%10;
	//jump to next
	if(index == 9) {
	    index--;
	}
	//add ','
	else if(index == 5 || index == 1) {
	    res[index] = ',';
	    index--;
	}

	res[index] = last + '0';
	index--;
	cents = cents/10;
    }

    return res;
}

int is_dir(char *path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}
