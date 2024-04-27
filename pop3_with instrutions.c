#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <dirent.h>
#define POP3SERVPORT 110  
#define MAX 80000
#define backspace 127

int sockfd;
char Serv_Host[MAX];
struct hostent *host;
struct sockaddr_in serv_addr;

int ctl;
int choice;
char buf[MAX];
char *pwd;
char filename[256];
char order1[256];
char search[256];
char subject[256];
char str[MAX]={0};
char message[256];
char DirPath[256];

//add the order here
char BLANK[MAX]="";
char ORDER[MAX];
char USER[MAX]="USER ";
char PASS[MAX]="PASS ";
char LIST[MAX]="LIST\n";
char STATUS[MAX]="STAT\n";
char RETR[MAX]="RETR ";
char DELE[MAX]="DELE";
char QUIT[MAX]="QUIT\n";

char RETRINFO[MAX]={0};
char RETREMAIL[MAX]={0};

void inputdomain(){
     while (1)
    {
        printf("Please input the pop3 domain: \n");//let the user input the domain name
        printf(">>");
        scanf("%s",buf); 
        getchar();
        strcpy(Serv_Host,buf); //get the host name
        
        if((host = gethostbyname(Serv_Host)) == NULL){   //set the host name
            printf("Can't find your host!\n");
            exit(1);
        }
        else{
            break;
        }
    }
    
    if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1){   //initialize the socket
        perror("socket error:");
        exit(1);
    }

    serv_addr.sin_family = PF_INET;
    serv_addr.sin_port = htons(POP3SERVPORT);
    serv_addr.sin_addr = *((struct in_addr *)host->h_addr); //input the port number
    bzero(&(serv_addr.sin_zero), 8);

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1){   //connet the server
        perror("connect error:");
        exit(1);
    }
    
    ctl=recv(sockfd,buf,MAX,0);   //receive the message from the server
    buf[ctl]='\0';
    printf("%s\n",buf);
    sleep(2);
}

void inputpassword(char *p){
    int i=0;

    system("stty -icanon");  //set the getchar() function in linux
    system("stty -echo");
    while(i<20)
    {
        p[i]=getchar();
        if(p[i]=='\n')
        {
            p[i]='\0';
            break;
        }
        else if(p[i]==backspace)  //use backspace to modify the input
        {
            if(i==0)
            {
                printf("\a");
                continue;
            }
            else{
                printf("\b \b");
                p[i]='\0';
                --i;
            } 
        }
        else
        {
            i++;
            printf("*");     //use * to replace the password
        }
    }

    system("stty echo");
    system("stty icanon");
}

void inputaccount(){
    system("clear");
    memset(buf,0,MAX);
    printf("PLeae input username: \n");
    printf(">>");
    scanf("%s",buf);
    getchar();

    strcat(USER,buf);            //input the account
    strcat(USER,"\n");
    if((send(sockfd,USER,strlen(USER),0))==-1)
    {
        printf("Error with your input account.\n");
        exit(1);
    }
    printf("-----------------------------\n");
    memset(buf,0,MAX);
    //printf("Waiting for connection..\n");
    ctl=recv(sockfd,buf,MAX,0);
    
    printf("%s\n",buf);

    memset(buf,0,MAX);
    printf("PLeae input password: \n");   //input the password
    printf(">>");
        
    inputpassword(buf);
    strcat(PASS,buf);
    strcat(PASS,"\n");
    if (send(sockfd,PASS,strlen(PASS),0)==-1)
    {
        printf("Error with your input password.\n");
    }
    printf("\n-----------------------\n");
    memset(buf,0,MAX);
    ctl=recv(sockfd,buf,MAX,0);
    printf("%s\n",buf);
    buf[ctl]='\0';
    sleep(2);
    
    fflush(stdin);
}

void showmenu(){
    system("clear");                                    //show the options
    printf("********************************************************\n");
    printf("1.Get a list of message and sizes\n");
    printf("2.Get mail status\n");
    printf("3.Display mail in detail\n");
    printf("4.Search text in all mails\n");
    printf("5.display by subjects\n");
    printf("6.Download the mail and delete in the remote service\n");
    printf("7.quit\n");
    printf("********************************************************\n");
    printf("PLease choose number: \n");
    printf(">>");
}

void listemail(){
    strcpy(ORDER,LIST);
    if ((send(sockfd,ORDER,strlen(ORDER),0))==-1)        //list the email content
    {
        printf("Fail to send the message to the mailbox!\n");
    }
    ctl=recv(sockfd,buf,MAX,0);                      //receive message from the socket
    if (ctl==0)
    {
        printf("No information is meaningful!\n");
    }
    else if (ctl==-1)
    {
        printf("Receiving process is wrong! Quit the program!");
        exit(1);
    }
    else
    {
        buf[ctl]='\0';
        printf("%s\n",buf);
    }
}

void statusmail(){                         //status of the email
    strcpy(ORDER,STATUS);
    if ((send(sockfd,ORDER,strlen(ORDER),0))==-1)
    {
        printf("Fail to send the message to the mailbox!\n");
    }
    ctl=recv(sockfd,buf,MAX,0);      //receive the message from the server
    if (ctl==0)
    {
        printf("No information is meaningful!\n");
    }
    else if (ctl==-1)
    {
        printf("Receiving process is wrong! Quit the program!");
        exit(1);
    }
    else
    {
        buf[ctl]='\0';
        printf("%s\n",buf);             //print the right information
    }
}

char *base64_decode(char *code)  
{  
    //based on base64 table to find the corresponding decimal digits
        int table[]={0,0,0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,62,0,0,0,
                63,52,53,54,55,56,57,58,
                59,60,61,0,0,0,0,0,0,0,0,
                1,2,3,4,5,6,7,8,9,10,11,12,
                13,14,15,16,17,18,19,20,21,
                22,23,24,25,0,0,0,0,0,0,26,
                27,28,29,30,31,32,33,34,35,
                36,37,38,39,40,41,42,43,44,
                45,46,47,48,49,50,51
                };  
        long len;  
        long str_len;  
        unsigned char *res;  
        int i,j;  
    
    //calculate the length after decoding
        len=strlen(code);  
        if(strstr(code,"=="))  
            str_len=len/4*3-2;  
        else if(strstr(code,"="))  
            str_len=len/4*3-1;  
        else  
            str_len=len/4*3;  
    
        res=malloc(sizeof(unsigned char)*str_len+1);  
        res[str_len]='\0';  
    
    //four symbols one time to decode
        for(i=0,j=0;i < len-2;j+=3,i+=4)  
        {  
            res[j]=((unsigned char)table[code[i]])<<2 | (((unsigned char)table[code[i+1]])>>4); //取出第一个字符对应base64表的十进制数的前6位与第二个字符对应base64表的十进制数的后2位进行组合  
            res[j+1]=(((unsigned char)table[code[i+1]])<<4) | (((unsigned char)table[code[i+2]])>>2); //取出第二个字符对应base64表的十进制数的后4位与第三个字符对应bas464表的十进制数的后4位进行组合  
            res[j+2]=(((unsigned char)table[code[i+2]])<<6) | ((unsigned char)table[code[i+3]]); //取出第三个字符对应base64表的十进制数的后2位与第4个字符进行组合  
        }  
    
        return res;  
}  

void email_replace(char * cp, int n, char * str)
{
	int lenofstr;
	int i;
	char * tmp;
	lenofstr = strlen(str); 
	//str3 is shorter that str2, it moves forward
	if(lenofstr < n)  
	{
		tmp = cp+n;
		while(*tmp)
		{
			*(tmp-(n-lenofstr)) = *tmp; //n-lenofstr是移动的距离 
			tmp++;
		}
		*(tmp-(n-lenofstr)) = *tmp; //move '\0'	
	}
	else if(lenofstr > n) //str3 is longer than str2, it moves backward
		{
			tmp = cp;
			while(*tmp) tmp++;
			while(tmp>=cp+n)
			{
				*(tmp+(lenofstr-n)) = *tmp;
				tmp--;
			}   
		}
	strncpy(cp,str,lenofstr);
}

void showemail(char *str1)
{
    char email[10000];
	char begin[100]="base64\r\n\r\n";  //the symbol of the beginning of the content
    char over[100]="\r\n------=_NextPart_";	   //the symbol of the end of the content
   	int i,len,count=0;
   	char c;
   	char *p=str1;  
    char *p1;	
    char *buf,*temp;
	
	//find the start of the string
   	p = strstr(str1,begin);
    
    while (p)
    {
        p+=strlen(begin);
        temp=p;
        p1=strstr(p,over);
        count=0;
        while (p<p1)
        {
            if (*p!='\r'&&*(p+1)!='\n')  //kick out the \r and \n in the content so that the decode can go smoothly
            {
                email[count]=*p;
                p+=1;
                count++;
            }
            else
            {
                p+=2;
                
            }
            
        }
        email[count]='\0';
        buf=base64_decode(email);
        email_replace(temp,strlen(email),buf);   //replace the content
        free(buf);
        p = strstr(p1,begin);
        
    }
    printf("-------------------EMAIL-----------------------\n");
}

void retremail(){
    fflush(stdin);
    printf("Input email number to see it in detail:\n");
    printf(">>");
    scanf("%s",buf);
    getchar();
    
    strcpy(ORDER,RETR);   //send the retrieve order of the email
    strcat(ORDER,buf);
    strcat(ORDER,"\n");
    if ((send(sockfd,ORDER,strlen(ORDER),0))==-1)
    {
        printf("Fail to send the message to the mailbox!\n");
    }

    printf("Waiting for Receiving...\n");
    sleep(3);

    ctl=recv(sockfd,RETRINFO,MAX,0);
    if (ctl==0)
    {
        printf("No useful information is got.!\n");
    }
    else if (ctl==-1)
    {
        printf("Receiving process is wrong! Quit the program!");
        exit(1);
    }
    else
    {
        while (1)           //sometimes the datagram can't be received in one time, so we need to wait until the '.' appears.
        {
            if ((RETRINFO[ctl-3]!='.')&&(RETRINFO[ctl-2]!='\r')&&(RETRINFO[ctl-1]!='\n'))
            {
                strcat(RETREMAIL,RETRINFO);
                memset(RETRINFO,0,MAX);
                ctl=recv(sockfd,RETRINFO,MAX,0);
            }
            else{
                RETRINFO[ctl]='\0';   //When '.' appears, the retrieving process is over.
                strcat(RETREMAIL,RETRINFO);
                showemail(RETREMAIL);
                printf("%s",RETREMAIL);
                break;
            }
            
        }
    }
}

void searchtext()
{
    printf("Input text you want to search:\n");
    scanf("%s",search);
    //Get the path
    if (getcwd(DirPath, 256) == NULL) {
        printf("Can't get the path.\n'");
    }
    char FileExtName1[] = ".eml";
    DIR *dir1;
    struct dirent *ptr1;

    if ((dir1 = opendir(DirPath)) == NULL) {
        perror("Can't open the dir.");
        exit(1);
    }
    char *CurFileExtName1 = NULL;
    //When find a file with ".eml", open it.
    while ((ptr1 = readdir(dir1)) != NULL) {
        if (strcmp(ptr1->d_name, ".") == 0 || strcmp(ptr1->d_name, "..") == 0)
            continue;
        else if (ptr1->d_type == 8) {//type is file
            CurFileExtName1 = rindex(ptr1->d_name, '.');
            if (CurFileExtName1 != NULL && strcmp(CurFileExtName1, FileExtName1) == 0) {
                FILE *p1 = fopen(ptr1->d_name, "r");  
                char ch;
                int j=0;
                //Get the message of file
                while ((ch = fgetc(p1)) != EOF) {
                    str[j] = ch;
                    j++;
                }
                int length = strlen(str);
                int i = 0;
                int searchnum=0;  //Ensure that only read text once
                while (i < length) {
                    if (searchnum == 1){
                        break;
                    }
                    //Find string "base64\r\n"
                    if (str[i - 1] == '\n' && str[i - 2] == '\r' && str[i - 3] == '\n' &&
                            str[i - 4] == '\r' && str[i - 5] == '4' && str[i - 6] == '6'
                            && str[i - 7] == 'e' && str[i - 8] == 's' && str[i - 9] == 'a' && str[i - 10] == 'b') {
                        searchnum = 1;
                        //Create a array to save message which should be decoded
                        char message[MAX] = {0};
                        int z = 0;
                        //retrive base 64 body and store them in char array body
                        while (!(str[i] == '\r' && str[i + 1] == '\n' &&
                                str[i + 2] == '\r' && str[i + 3] == '\n')) {
                            message[z++] = str[i++];
                        }
                        message[z] = '\0';//end with \0
                        char *dstr;
                        //Do base64 decode
                        dstr = base64_decode(message);
                        //Find the text user want to search in message that be decoded
                        if(strstr(dstr,search) == NULL) {
                            printf("%s doesn't have %s\n", ptr1->d_name,search);
                            fclose(p1);
                            continue;
                        }
                        else{
                            printf("%s have %s\n", ptr1->d_name,search);
                            fclose(p1);
                            continue;
                        }
                        printf("\n");   
                        }
                        else{//if not the base 64 body ,just copy received message
                                i++;
                        }
                }
                
            }           
        }
    }
    closedir(dir1);
}

void showsubject()
{
    //Get the path
    if (getcwd(DirPath, 256) == NULL) {
        printf("Can't get the path.\n'");
    }
    char FileExtName[] = ".eml";
    DIR *dir;
    struct dirent *ptr;

    if ((dir = opendir(DirPath)) == NULL) {
        perror("Can't open the dir.");
        exit(1);
    }
    char *CurFileExtName = NULL;
    //When find a file with ".eml", open it.
    while ((ptr = readdir(dir)) != NULL) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            continue;
        else if (ptr->d_type == 8) {//type is file
            CurFileExtName = rindex(ptr->d_name, '.');
            if (CurFileExtName != NULL && strcmp(CurFileExtName, FileExtName) == 0) {
                FILE *p = fopen(ptr->d_name, "r");  
                char ch;
                int i=0;
                while ((ch = fgetc(p)) != EOF) {
                    str[i] = ch;
                    i++;
                }
                //Match the string "Subject:"
                if (strstr(str, "Subject: ") == NULL) {
                    printf("%s don't have subject\n", ptr->d_name);
                    fclose(p);
                    continue;
                }
                //Get the message after "Subject:"
                i = strstr(str, "Subject: ") - str + 9;
                printf("The subject of %s is ", ptr->d_name);
                while (str[i] != '\r' && str[i + 1] != '\n') {
                    printf("%c", str[i]);
                    i++;
                }
                printf("\n");   
                fclose(p); 
            }           
        }
    }
    closedir(dir);
}

void downloademail()
{
    fflush(stdin);
    //Get the mail user want to download
    printf("Input email number to storage email you want:\n");
    printf("mypop3>>");
    scanf("%s",order1);
    getchar();
    strcpy(ORDER,RETR);
    strcat(ORDER,order1);
    strcat(ORDER,"\n");
    printf("The email you want to save is %s\n",order1);
    if ((send(sockfd,ORDER,strlen(ORDER),0))==-1)
    {
        printf("Fail to send the message to the mailbox!\n");
    }

    printf("Waiting for Receiving...\n");
    sleep(5);

    ctl=recv(sockfd,RETRINFO,MAX,0);
    if (ctl==0)
    {
        printf("No useful information is got.!\n");
    }
    else if (ctl==-1)
    {
        printf("Receiving process is wrong! Quit the program!");
        exit(1);
    }
    else
    {
        //Input the file name
        printf("Please input filename you want to save:\n");
        scanf("%s",filename);
        strcat(filename,".eml");
        //Open a new file to save mail
        FILE* fp2;
        fp2 = fopen(filename,"wb");
        fwrite(RETRINFO,1,strlen(RETRINFO),fp2);
        fflush(fp2);
        fclose(fp2);
        strcat(DELE," ");
        strcat(DELE,order1);
        strcat(DELE,"\n");
        //Delete mail which has already downloaded
        send(sockfd,DELE,strlen(DELE),0);
            printf("Waiting for Delete...\n");
        sleep(5);
        printf("%s save successfully!\n",filename);
    }
}

void quitemail(){
    strcpy(ORDER,BLANK);
    strcpy(ORDER,QUIT);
    if ((send(sockfd,ORDER,strlen(ORDER),0))==-1)
    {
        printf("Fail to send the message to the mailbox!\n");
    }
    ctl=recv(sockfd,buf,MAX,0);
    if (ctl==0)
    {
        printf("No information is meaningful!\n");
    }
    else if (ctl==-1)
    {
        printf("Receiving process is wrong! Quit the program!");
        exit(1);
    }
    else
    {
        buf[ctl]='\0';
        printf("%s\n",buf);
        printf("Quit successfully!\n");
    }
}
int main()
{
   inputdomain();
   inputaccount();

    while (1)
    {
        showmenu();
        scanf("%d",&choice);
        getchar();
        memset(buf,0,MAX);
        //Input order to use different function
        switch (choice)
        {
            case 1:
                listemail();
                break;
            
            case 2:
                statusmail();
                break;
            case 3:
                retremail();
                break;
            
            case 4:
                searchtext();
                break;
            case 5:
                showsubject();
                break;
            case 6:
                downloademail();
                break;
            case 7:
                quitemail();
                break;
            default:
                while (1)
                {
                    printf("Press 1 to return to interface.\n");
                    printf("Press 2 to return to quit\n");
                    printf("mypop3>>");
                    scanf("%d",&choice);
                    if (choice==1)
                    {
                        fflush(stdin);
                        break;
                    }
                    else if(choice==2)
                    {
                        exit(1);
                    }
                    else{
                        fflush(stdin);
                    }
                }
                break;
        }

        while (1)
        {
            strcpy(ORDER,BLANK);
            printf("Press 1 to return to interface.\n");
            printf("Press 2 to return to quit\n");
            printf(">>");
            scanf("%d",&choice);
            if (choice==1)
            {
                fflush(stdin);
                break;
            }
            else if(choice==2)
            {
                exit(1);
            }
            else{
                fflush(stdin);
            }
        }
    }
    
    close(sockfd);
    return 0;
}





