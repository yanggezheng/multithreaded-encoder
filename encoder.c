#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#define THREAD_NUM 3
int taskCount = 0, count = 0, end = 0, taskIndex = 0, totalTask = 0, writeCount = 0, finish = 0;
pthread_mutex_t mutexQueue, mutexWrite;
pthread_cond_t condQueue, condWrite;
void part1(int argc, char **argv)
{
    int fd = open(argv[1], O_RDONLY), i = 1, j = 2;
    struct stat sb;
    if (fstat(fd, &sb) == -1)
    {
        perror("couldn't get file size.\n");
    }
    char *buf = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    char prevous = buf[0];
    int count = 1;
    for (; i < sb.st_size; i++)
    {
        if (prevous ^ buf[i])
        {
            write(1, &prevous, 1);
            write(1, &count, 1);
            count = 1;
            prevous = buf[i];
            continue;
        }
        count++;
    }
    munmap(buf, sb.st_size);
    for (; j < argc; j++)
    {
        fd = open(argv[j], O_RDONLY);
        buf = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        for (i = 0; i < sb.st_size; i++)
        {
            if (prevous ^ buf[i])
            {
                write(1, &prevous, 1);
                write(1, &count, 1);
                count = 1;
                prevous = buf[i];
                continue;
            }
            count++;
        }
        munmap(buf, sb.st_size);
    }
    write(1, &prevous, 1);
    write(1, &count, 1);
}
typedef struct Task
{
    int index, size;
    unsigned char *buf;
} Task;
Task *taskQueue[300000];
Task **resQueue;

void executeTask(Task *task)
{

    char *result = malloc(sizeof(char) * 8194);
    unsigned char *str = task->buf;
    unsigned char count = 0;
    int j = 0, i = 0;
    char previous = str[0];
    // printf("sgfas %d\n", task->size);
    for (; j < task->size; j++)
    {
        char current = str[j];
        if (current == previous)
        {
            count++;
        }
        else
        {
            result[i] = previous;
            // printf("result added %c\n", result[i]);
            result[i + 1] = count;
            // printf("result added %c\n", result[i]);
            previous = current;
            count = 1;
            i += 2;
        }
    }
    result[i++] = previous;
    result[i++] = count;
    // printf("the res is %c\n", result[0]);
    // printf("the res is %c\n", result[1]+40);
    // printf("the res is %c\n", result[2]);
    // printf("the res is %c\n", result[3]+40);
    // printf("the res is %c\n", result[4]);
    // printf("the res is %c\n", result[5]+40);
    // printf("sgf %d\n", 48);
    // if (str[task->size - 2] != str[task->size - 1])
    // {
    //     result[i++] = str[task->size - 1];
    //     result[i++] = '\1';
    // }
    Task *newTask = malloc(sizeof(Task));
    // printf("sgf %d\n", 55);
    Task temp = {.buf = (unsigned char *)result, .size = i, .index = -1};
    *newTask = temp;
    // pthread_mutex_lock(&mutexWrite);
    // printf("sgf %d\n", 59);
    // printf("resQueue pointer %p\n", resQueue);
    resQueue[task->index] = newTask;
    // pthread_mutex_unlock(&mutexWrite);
    // printf("YG%s\n", resQueue[task->index]->buf);
    // printf("YG%d\n", resQueue[task->index]->size);
    // printf("YG%d\n", resQueue[task->index]->index);
    // pthread_cond_signal(&condWrite);
}

void submitTask(Task *task)
{

    taskQueue[task->index] = task;
    taskCount++;
    // totalTask++;
    // pthread_cond_signal(&condQueue);
}

void *startThread(void *args)
{
    while (1)
    {   
        Task task;
        pthread_mutex_lock(&mutexQueue);

        if (taskIndex < taskCount)
        {
            task = *taskQueue[taskIndex++];
        }
        else
        {   pthread_mutex_unlock(&mutexQueue);
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&mutexQueue);
        executeTask(&task);
    }
}

void writeTo()
{
    int i = 0;
    Task *task, *next;
    for (int j = 0; j < taskCount - 1; j++)
    {   
        // pthread_mutex_lock(&mutexWrite);
        // while (resQueue[writeCount] == NULL && finish == 0)
        // {
        //     pthread_cond_wait(&condWrite, &mutexWrite);
        // }
        // if (resQueue[writeCount] != NULL && resQueue[writeCount + 1] != NULL)
        // {
        task = resQueue[writeCount];
        next = resQueue[writeCount + 1];

        fwrite(&task->buf[i], task->size - 2, 1, stdout);

        if (task->buf[task->size - 2] == next->buf[0])
        {
            next->buf[1] += task->buf[task->size - 1];
        }
        else
        {
            fwrite(&task->buf[task->size - 2], 1, 1, stdout);
            fwrite(&task->buf[task->size - 1], 1, 1, stdout);
        }
        writeCount++;
        // }
        // else if (finish == 1)
        // {
        if (writeCount == totalTask - 2)
        {
            task = resQueue[writeCount];
            for (i = 0; i < task->size; i++)
            {
                fwrite(&task->buf[i], 1, 1, stdout);
            }
            //         // pthread_mutex_unlock(&mutexWrite);
            //         break;
            //     }
        }
        // pthread_mutex_unlock(&mutexWrite);
    }
    task = resQueue[taskCount - 1];
    for (i = 0; i < task->size; i++)
    {
        fwrite(&task->buf[i], 1, 1, stdout);
    }
}

int main(int argc, char **argv, char **envp)
{   
    // int opt = getopt(argc, argv, "j:");
    // if (opt == -1)
    // {
    // }
    pthread_t ts[THREAD_NUM];

    resQueue = malloc(300000*sizeof(Task*));
    pthread_mutex_init(&mutexQueue, NULL);
    // pthread_cond_init(&condQueue, NULL);
    // pthread_mutex_init(&mutexWrite, NULL);
    // pthread_cond_init(&condWrite, NULL);
    int i, j, k, fd, count1 = 0;
    unsigned char *buf;
    if (strcmp(argv[1], "-j") == 0)
    {
        i = 3;
    }
    else
    {
        part1(argc, argv);
        return 0;
    }
    int numberOfFile = argc - i, init = i;
    struct stat sb[numberOfFile];
    for (; i < argc; i++)
    {
        fd = open(argv[i], O_RDONLY);
        if (fd == -1)
        {
            perror("error opening the filec");
            return 1;
        }
        fstat(fd, &sb[i - init]);
        buf = mmap(NULL, sb[i - init].st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        // printf("I am here %d\n", 191);
        // write(1, buf, sb[i - init].st_size);
        // return 0;
        int number = ((((int)sb[i - init].st_size) - 1) / 4096) + 1;
        for (j = 0; j < number; j++)
        {
            Task *worker = malloc(sizeof(Task));
            if (j != number - 1)
            {
                Task task = {
                    .index = count++,
                    .size = 4096,
                    .buf = &buf[j * 4096]};
                *worker = task;
                submitTask(worker);
            }
            else
            {
                Task task = {
                    .index = count++,
                    .size = ((int)sb[i - 3].st_size) - j * 4096,
                    .buf = &buf[j * 4096]};
                *worker = task;
                submitTask(worker);
                // if (i == argc - 1)
                // {
                //     // printf("the value of i is %d\n", i);
                //     // printf("the value of argc is %d\n", argc);
                //     end = 1;
                //     // printf("kaka\n");
                // }
            }
            // pthread_cond_broadcast(&condQueue);
        }
        close(fd);
    }
    // for (i = 0; i < taskQueue[0]->size; i++)
    // {
    //     printf("229 %c\n", *(taskQueue[0]->buf + i));
    // }
    // printf("231 %d\n", taskQueue[0]->index);

    // return 0;
    // printf("asdasd\n");

    for (i = 0; i < THREAD_NUM; i++)
    {
        // printf("I am startthread %d\n", 223);
        if (pthread_create(&ts[i], NULL, startThread, NULL) != 0)
        {
            perror("Failed to create the thread");
        }
    }
    // printf("asdaasdasdasdsd\n");

    for (j = 0; j < THREAD_NUM; j++)
    {
        pthread_join(ts[j], NULL);
    }
    // 这里挂了
    writeTo();

    // printf("qweqweqwewqew6788\n");
    pthread_mutex_destroy(&mutexQueue);
    // pthread_cond_destroy(&condQueue);
    // pthread_mutex_destroy(&mutexWrite);
    // pthread_cond_destroy(&condWrite);
    // free(resQueue);
    return 0;
}
