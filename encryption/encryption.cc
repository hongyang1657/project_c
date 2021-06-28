#include<stdio.h> 
#include<stdlib.h> 
#include<string.h> 
#include<iostream>
#include<cstdio>

using namespace std;

void encfile(char *in_filename,char *pwd,char *out_filename);/*对文件进行加密的具体函数*/ 
void decryptfile(char* in_filename,char *pwd,char *out_filename); /*对文件解密的具体函数*/
void deleteFile(char *filename);

char *project = "a.out";
char *in_filename = "sourcefile.txt";/*用户输入的要加密的文件名 或是 要解密的文件*/ 
char *out_filename = "encryptedfile.txt"; /*用户输入加密后保存的文件名 或是解密保存的文件名*/
char *pwd = "fitme";/*用来保存密码*/ 

int main(int argc,char *argv[])/*定义main()函数的命令行参数*/ 
{ 
	int option; /*功能选择*/
	

	printf("thank you for using this program......\n");
	printf("1. Encrypt a file         2. Decrypt a file\n");	
	printf("chose your option.....\n");

	
	scanf("%d",&option);
	getchar();
	if(argc!=4){/*容错处理*/ 
		printf("Please input In-filename:\n"); 
		//gets(in_filename);/*得到要加密的文件名*/ 
		printf("Please input your Password:\n"); 
		//gets(pwd);/*得到密码*/ 
		printf("Please input Out-filename:\n"); 
		//gets(out_filename);/*得到加密后你要的文件名*/ 
		
	} 
	else{/*如果命令行参数正确,便直接运行程序*/ 
		strcpy(in_filename,argv[1]); 
		strcpy(pwd,argv[2]); 
		strcpy(out_filename,argv[3]); 
	} 

	switch(option){
	case 1: //加密	
		encfile(in_filename,pwd,out_filename);/*加密函数调用*/ 
		break;
	case 2://解密
		decryptfile(out_filename,pwd,in_filename);/*解密函数调用*/
		break;
	default:
		break;
	}	

    
    deleteFile(project);
	
	return 0; 
} 

/*加密子函数开始*/ 
void encfile(char *in_filename,char *pwd,char *out_file) 
{ 
	FILE *fp1,*fp2; 
	register char ch; 
	int j=0; 
	int j0=0; 
	fp1=fopen(in_filename,"r");/*打开要加密的文件*/ 
	if(fp1==NULL){ 
		printf("cannot open in-file.\n"); 
		exit(1);/*如果不能打开要加密的文件,便退出程序*/ 
	} 
	fp2=fopen(out_file,"w"); 
	if(fp2==NULL){ 
		printf("cannot open or create out-file.\n"); 
		exit(1);/*如果不能建立加密后的文件,便退出*/ 
	} 

	while(pwd[++j0]);  

	ch=fgetc(fp1); 

	/*加密算法开始*/ 
	while(!feof(fp1)){  //feof()检查流上文件的结束标识(是否读到文件结尾)
		if(j0 >7)
		   j0 = 0;
		ch += pwd[j0++];
		fputc(ch,fp2); 
		ch=fgetc(fp1); 
	} 
	fclose(fp1);/*关闭源文件*/ 
	fclose(fp2);/*关闭目标文件*/ 
    //删除未加密的源文件
    deleteFile(in_filename);
} 

/*解密子函数开始*/ 
void decryptfile(char *in_filename,char *pwd,char *out_file) 
{ 
	FILE *fp1,*fp2; 
	register char ch; 
	int j=0; 
	int j0=0; 
	fp1=fopen(in_filename,"r");/*打开要解密的文件*/ 
	if(fp1==NULL){ 
		printf("cannot open in-file.\n"); 
		exit(1);/*如果不能打开要解密的文件,便退出程序*/ 
	} 
	fp2=fopen(out_file,"w"); 
	if(fp2==NULL){ 
		printf("cannot open or create out-file.\n"); 
		exit(1);/*如果不能建立解密后的文件,便退出*/ 
	} 
	
	while(pwd[++j0]);  	
	ch=fgetc(fp1); 
	/*解密算法开始*/ 
	while(!feof(fp1)){ 
		if(j0 >7)
		   j0 = 0;
		ch -= pwd[j0++];
		fputc(ch,fp2);/*我的解密算法*/
		ch=fgetc(fp1); 
	} 
	fclose(fp1);/*关闭源文件*/ 
	fclose(fp2);/*关闭目标文件*/ 
} 

void deleteFile(char *filename){
    if(remove(filename)==0){
        cout<<"删除成功"<<endl;        
    }
    else{
        cout<<"删除失败"<<endl;        
    }
}