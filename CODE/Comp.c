#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

//команды
#define ADD       0x0
#define AND       0x1
#define OR        0x2
#define XOR       0x3
#define SUB       0x4
#define LOAD      0x5
#define OUT       0x6
#define JUMP       0x7
#define STORE_RAM 0x8 // в RAM
#define LOAD_RAM  0x9 // из RAM
#define READ_TOG  0x10 // читаем переключатели

#define MAX_PROG_SIZE     192
#define MAX_STR_LENGHT    64
#define MAX_LABELS        32
#define MAX_LABELS_LENGHT 16

//когда пишешь команду в cmd, то пропиши сначала это   chcp 65001
//это позволит видеть русский текст, а не кракозябры

// [System.IO.File]::ReadAllBytes("code.bin") | ForEach-Object { [Convert]::ToString($_, 2).PadLeft(8, '0') } | Out-File -FilePath "code.txt" -Encoding ASCII      это команда превращает наш code.bin в текстовый документ (это для того, чтобы удобнее было работать с Multisim)
typedef struct
{
	char name [MAX_LABELS_LENGHT];
	int address;
}Label;

typedef struct 
{
	uint8_t code[MAX_PROG_SIZE]; 
	int size;
}Program; 

int parse_line(const char* line, uint8_t* opcode, uint8_t* operand){ // парсим||читаем строку (скипая пробелы)
	char command[32];
	int value = 0;

	while(isspace(*line)) line++;
	if(*line == ';' || *line == '\0') return 0;
	sscanf(line, "%31s %d", command, &value);

	if(strcmp(command, "ADD") == 0){    //сравниваем написанную команду в нашем коде с базой комманд
		*opcode = ADD;                  //это мы записываем в старшие 4 бита
		*operand = value & 0xF;         //а это в младшие 4 бита
		return 1;						//1 = всё ок, 0 = такой команды не существует
	}
	else if(strcmp(command, "AND") == 0){
		*opcode = AND;
		*operand = value & 0xF;
		return 1;
	}
	else if(strcmp(command, "OR") == 0){
		*opcode = OR;
		*operand = value & 0xF;
		return 1;
	}
	else if(strcmp(command, "XOR") == 0){
		*opcode = XOR;
		*operand = value & 0xF;
		return 1;
	}	
	else if(strcmp(command, "SUB") == 0){
		*opcode = SUB;
		*operand = value & 0xF;
		return 1;
	}
	else if(strcmp(command, "LOAD") == 0){
		*opcode = LOAD;
		*operand = value & 0xF;
		return 1;
	}
	else if(strcmp(command, "OUT") == 0){
		*opcode = OUT;
		*operand = value & 0xF;
		return 1;
	}
	else if(strcmp(command, "JUMP") == 0){
		*opcode = JUMP;
		*operand = value & 0xF;
		return 1;
	}
	else if(strcmp(command, "STORE_RAM") == 0){
		*opcode = STORE_RAM;
		*operand = value & 0xF;
		return 1;
	}
	else if(strcmp(command, "LOAD_RAM") == 0){
		*opcode = LOAD_RAM;
		*operand = value & 0xF; 
		return 1;
	}
	else if(strcmp(command, "READ_TOG") == 0){
		*opcode = READ_TOG;
		*operand = value & 0xF;
		return 1;
	}
}

int compile(const char* input_file, const char* output_file){  // команда в cmd должна выглядеть так(показываю вторую строчку, первая должна быть gcc Comp.c -0 Comp): ./Comp <название_входного_файла>.asm <название_выходного_файла>.bin
	FILE* in = fopen(input_file, "r");
	if(!in){  // чекаем, можем ли мы открыть файл
		printf("Ошибка: не удалось открыть файл %s\n", input_file);
		return 1;                         //уже в данном случае 1 = плохо, а 0 = хорошо
	}

	Program program = {0};
	char line[MAX_STR_LENGHT];
	int line_num = 0;

	while(fgets(line, sizeof(line), in)){
		line_num++;
		uint8_t opcode, operand;

		if(parse_line(line, &opcode, &operand)){
			program.code[program.size++] = (opcode << 4) | operand; //это мы формируем саму строку, то есть [команда:4бита][значение:4бита]

			if(program.size >= MAX_PROG_SIZE){
				printf("Ошибка: программа слишком большая\n");
				fclose(in);
				return 1;
			}
		}
	}
	fclose(in);

	//начинаем формировать сам файлик
	FILE *out = fopen(output_file, "wb");                          // wb тип wildberries, хе-хе
	if(!out){
		printf("Ошибка: не удалось создать файл %s\n");
		return 1;
	}
	fwrite(program.code, 1, program.size, out);
	fclose(out);

	printf("Всё ок! %d байт записано в %s\n", program.size, output_file);
	return 0;
}

int main(int argc, char* argv[]){
	if(argc != 3){
		printf("Использование: %s <input.asm> <output.bin>\n", argv[0]);
		return 1;
	}

	return compile(argv[1], argv[2]);
}