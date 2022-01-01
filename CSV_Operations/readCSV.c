#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define NEW_LINE printf("\n");

//Adjust according to .csv row size.
//Not enforced in the program.
#define MAX_BUFFER 10000
#define MAX_COLUMNS 1000
typedef const unsigned int index_t;

typedef struct f32_dataframe
{
    char* colNames;
    size_t* colIndices;
    float* data;
    size_t nrow;
    size_t ncol;
} f32_dataframe_t;

f32_dataframe_t f32_readCSV(const char* fileName, index_t memAlloc, bool hasHeader)
{
    FILE* file_p = fopen(fileName, "r");
    if (file_p == NULL)
    {
        printf("%s failed to open. \n", fileName);
        exit(1);
    }

    f32_dataframe_t dataframe;
    
    if (hasHeader)
    {
        char columns[MAX_BUFFER] ; size_t indices[MAX_COLUMNS];
        indices[0] = 0;
        size_t nrow = 0, ncol = 0;
        size_t colStartIndex = 0, strLength;
        fgets(columns, MAX_BUFFER, file_p);
        char* token = strtok(columns, ",");
        while (token != NULL)
        {   
            strLength = strlen(token);
            indices[colStartIndex + 1] =  strLength + indices[colStartIndex]; 
            for (size_t i = 0; i < strLength; i++)
            {
                *(columns + indices[colStartIndex] + i) = *(token + i);
            }
            colStartIndex++;
            token = strtok(NULL, ",");
        }
        indices[colStartIndex]--; 
        index_t nullIndex = indices[colStartIndex]; 
        *(columns + nullIndex) = '\0'; 
        dataframe.colNames = (char*)malloc(nullIndex);
        strcpy(dataframe.colNames, columns);
        index_t nBytes = sizeof(size_t) * (colStartIndex + 1);
        dataframe.colIndices = (size_t*)malloc(nBytes);
        memcpy(dataframe.colIndices, indices, nBytes);
    }
    else
    {   dataframe.colNames = NULL; dataframe.colIndices = NULL; }
    
    float* data = (float*)malloc(sizeof(float) * memAlloc);
    size_t elemCounter = 0, nrow = 0, ncol = 0;
    char line[MAX_BUFFER]; char* token;
    while (fgets(line, MAX_BUFFER, file_p) != NULL)
    {   
        nrow++;
        token = strtok(line, ",");
        while (token != NULL)
        {   
            *(data + elemCounter) = strtof(token, NULL);
            elemCounter++;
            token = strtok(NULL, ",");
        }
    }
    fclose(file_p);
    nrow--;
    ncol = elemCounter / nrow;

    dataframe.data = data;
    dataframe.nrow = nrow; dataframe.ncol = ncol;
    return dataframe;
}

void f32_freeCSV(f32_dataframe_t* data_p)
{
    free(data_p->colNames);
    free(data_p->colIndices);
    free(data_p->data);
}

void f32_printColumnNames(f32_dataframe_t* data_p)
{
    size_t start, end;
    for (size_t i = 0; i < data_p->ncol; i++)
    {   
        start = *(data_p->colIndices + i); end = *(data_p->colIndices + i + 1); 
        for (size_t j = start; j < end; j++)
        {   printf("%c", *(data_p->colNames + j));  }
        printf(" ");
    } 
    NEW_LINE;
}

void f32_printData(f32_dataframe_t* data_p)
{
    for (size_t i = 0; i < data_p->nrow; i++)
    {   
        for (size_t j = 0; j < data_p->ncol; j++)
        {   printf("%f ", *(data_p->data + i*data_p->ncol + j));  }
        NEW_LINE;
    }
}

float* f32_getRow(f32_dataframe_t* data_p, const size_t row)
{
    float* row_p;
    if (row < data_p->nrow)
    {
        const size_t ncol = data_p->ncol;
        row_p = (float*)malloc(sizeof(float) * ncol);
        for (size_t i = 0; i < ncol; i++)
        {   row_p[i] = *(data_p->data + (row * ncol) + i);  }
    }
    else
    {   row_p = NULL;   }
    return row_p;
}

float* f32_getCol(f32_dataframe_t* data_p, const size_t col)
{
    float* col_p;
    if (col < data_p->ncol)
    {
        const size_t nrow = data_p->nrow;
        col_p = (float*)malloc(sizeof(float) * nrow);
        for (size_t i = 0; i < nrow; i++)
        {   col_p[i] = *(data_p->data + (i * data_p->ncol) + col);  }
    }
    else
    {   col_p = NULL;   }
    return col_p;
}

int main(void)
{   
    const char* fileName = "/path/to/numbers.txt";
    //memAlloc can be larger than the actual number of elements in the .csv file, but not smaller.
    index_t memAlloc = 100; bool hasHeader = true;
    
    f32_dataframe_t data = f32_readCSV(fileName, memAlloc, hasHeader);
    
    f32_printColumnNames(&data);

    f32_printData(&data);

    float* row1 = f32_getRow(&data, 1);
    if (row1)
    {   
        printf("Row 1: \n");
        for (size_t i = 0; i < data.ncol; i++)
        {   printf("%f ", row1[i]); }
        NEW_LINE;
    }

    float* col4 = f32_getCol(&data, 4);
    if (col4)
    {
        printf("Column 4: \n");
        for (size_t i = 0; i < data.nrow; i++)
        {   printf("%f ", col4[i]); }
        NEW_LINE;
    }

    f32_freeCSV(&data);
    free(row1);
    free(col4);
    
    return 0;
}
