//#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <opencv2/opencv.hpp>
 
#define MAXTITL 40
#define MAXAUTL 40
#define MAXBKS 10
 
char *s_gets(char*st, int n);
// book模板
struct book {
	char title[MAXTITL];
	char author[MAXAUTL];
	float value;
};
 
int main()
{
	struct book library[MAXBKS];
	int count = 0;
	int index, filecount;
	FILE * pbooks;
	int size = sizeof(struct book);
	// 获取文件读写权限
	if ((pbooks = fopen("book.dat", "a + b")) == NULL) {
		fputs("cann not open book.dat file\n", stderr);
		exit(1);
	}
 
	// 定位到文件开始
	rewind(pbooks);
	while (count < MAXBKS && fread(&library[count], size, 1, pbooks) == 1) {
		if (count == 0)
			puts("current contents of book.dat:");
		printf("%s by %s:$%.2f\n", library[count].title, library[count].author, library[count].value);
		count++;
	}
	filecount = count;
	if (count == MAXBKS) {
		fputs("the book.dat file is full.", stderr);
		exit(2);
	}
 
	puts("Please add new book titles.");
	puts("press enter at the start of a linne to stop.");
	while (count < MAXBKS && s_gets(library[count].title, MAXTITL) 
		!= NULL && library[count].title[0] != '\0') {
		puts("now enter the author.");
		s_gets(library[count].author, MAXAUTL);
		puts("Now enter the value");
		scanf("%f", &library[count++].value);
		// 清理输入的末尾换行符
		while (getchar() != '\n')
			continue;
		if (count < MAXBKS)
			puts("Enter thr next title");
	}
 
	if (count > 0) {
		puts("Here is the list of your books:");
		for (index = 0; index < count; index++)
			printf("%s by %s:$%.2f\n", library[index].title, library[index].author, library[index].value);
		fwrite(&library[filecount], size, count - filecount, pbooks);
	}
	else {
		puts("No books? Too bad.\n");
	}
	fclose(pbooks);
 
	return 0;
}
// 用以消除字符输入回车带入的换行符
char *s_gets(char*st, int n) {
	char *ret_val;
	char *find;
 
	ret_val = fgets(st, n, stdin);
	// 查找换行符
	if (ret_val) {
		find = strchr(st, '\n');
		// 如果地址不是NULL
		if (find)
			// 此处放置一个空字符
			*find = '\0';
		else
			while (getchar() != '\n')
				// 清理输入行
				continue;
	}
	return ret_val;
}
