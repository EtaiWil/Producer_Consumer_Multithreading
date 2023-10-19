#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>


typedef struct {
    char** buffer; // array of pointers to strings
    int length; // number of elements in the buffer
    int nextInsertIndex;
    int nextDeleteIndex;
    sem_t empty; // hold the number of elements that are empty in the buffer
    sem_t full; // hold the number of elements that are full in the buffer
    pthread_mutex_t mutex;
    int numofElementInserted;
    int numOfElementRemoved;
    int numberOfProducer;
    int numofSports;
    int numofNews;
    int numofWeather;
    int flagForEndMessages;

} Bounded_Buffer;

typedef struct Node {
    char* data;
    struct Node* next;
} Node;

typedef struct Unbounded_Buffer {
    Node* head;
    Node* tail;
    pthread_mutex_t mutex;
    sem_t sem_full;
    int numofElementInserted;
    int numOfElementRemoved;
    int flagForEndMessages;
} Unbounded_Buffer;


typedef struct {
    int numberOfProducts; // his serial number prod1, prod2, etc.
    int queueSize; // size of the queue for the bounded queue.
    int numberOfElemnetNedded;
    Bounded_Buffer* bb; // pointer to the shared bounded buffer
} argsForProducer;

typedef struct {
    int queueSize;
    Bounded_Buffer** buffers; // array of pointers to the bounded buffer
    int numProducers; // number of producers
    Unbounded_Buffer* unboundedSport; // pointer to the unbounded sport buffer
    Unbounded_Buffer* unboundedNews; // pointer to the unbounded news buffer
    Unbounded_Buffer* unboundedWeather; // pointer to the unbounded weather buffer
} argsForConsumer;

typedef struct {
    Bounded_Buffer* boundedBuffer;
    Unbounded_Buffer* unboundedBuffer;
} argsForCoEditor;
typedef struct {
    int serialNumber;
    int queueSize  ;
     int numberOfElemnetNedded;
} Group;


Bounded_Buffer* create_bounded_buffer(int size, int numberOfProducer);
void destroy_bounded_buffer(Bounded_Buffer* bb);
char* remove_item(Bounded_Buffer* bb);
void* Dispatcher(void* args);

void insert(Bounded_Buffer* bb, char* s);
void* Producer(void* args);
Unbounded_Buffer* create_unbounded_buffer();

void insertToUnbounded(Unbounded_Buffer* ub, char* item);

char* removeFromUnBounded(Unbounded_Buffer* ub);
void *CoEditor(void *args);
void extract_category(const char* item, char* category);
void destroy_unbounded_buffer(Unbounded_Buffer* buffer);


void* screenManager(void* args);



int main(int argc, char *argv[]) {

    int fileSize = 0; // Variable to store the size of the file
    int numbers[3];
    int count = 0;
    int blockCount = 0;
    int lastRowValue = 0; // Variable to store the integer value of the last row

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }
    while (fscanf(file, "%d", &numbers[count]) == 1) {
        count++;
        if (count == 3) {
            blockCount++;
            count = 0;
        }
        lastRowValue = numbers[count - 1]; // Store the last number read in the file
        fileSize++;
    }

    fclose(file);

    int maxGroups = blockCount;
    Group groups[maxGroups];

    file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }

    int groupCount = 0;


    count = 0; // Reset the count
    while (fscanf(file, "%d", &numbers[count]) == 1) {
        count++;
        if (count == 3) {
            count = 0;

            Group newGroup;
            newGroup.serialNumber = numbers[0];
            newGroup.numberOfElemnetNedded = numbers[1];
            newGroup.queueSize = numbers[2];

            groups[groupCount] = newGroup;
            groupCount++;
        }
    }

    fclose(file);





    srand(time(NULL));


    int numProducers = maxGroups;



    Bounded_Buffer* buffers[numProducers];


    for (int i = 0; i < numProducers; ++i) {
        buffers[i] = create_bounded_buffer(groups[i].queueSize, groups[i].serialNumber);
    }

    pthread_t producerThreads[numProducers];
    // Create and start producer threads
    for (int i = 0; i < numProducers; i++) {
        argsForProducer* prodArgs = malloc(sizeof(argsForProducer));
        prodArgs->numberOfProducts = i + 1;
        prodArgs->queueSize = groups[i].queueSize;
        prodArgs->bb = buffers[i];
        prodArgs->numberOfElemnetNedded=groups[i].numberOfElemnetNedded;

        pthread_create(&producerThreads[i], NULL, Producer, prodArgs);
    }


    Unbounded_Buffer* unboundedSport = create_unbounded_buffer();
    Unbounded_Buffer* unboundedNews = create_unbounded_buffer();
    Unbounded_Buffer* unboundedWeather = create_unbounded_buffer();


    argsForConsumer consArgs;
    consArgs.buffers = buffers;
    consArgs.numProducers = numProducers;
    consArgs.unboundedSport = unboundedSport;
    consArgs.unboundedNews = unboundedNews;
    consArgs.unboundedWeather = unboundedWeather;
    pthread_t DispatcherThread;
    pthread_create(&DispatcherThread, NULL, Dispatcher, &consArgs);




    int sumOfAllproducers=lastRowValue;
    argsForCoEditor coEditorArgs[3];



    //first coedtior is sport, second is news, third is weather
    Bounded_Buffer* boundedBuffer = create_bounded_buffer(sumOfAllproducers, 1);//do 1 here becuase i dont care about it here;
    // Initialize the unbounded buffers for each coEditor
    coEditorArgs[0].unboundedBuffer = unboundedSport;
    coEditorArgs[1].unboundedBuffer =unboundedNews;
    coEditorArgs[2].unboundedBuffer = unboundedWeather;


    for (int i = 0; i < 3; i++) {
        coEditorArgs[i].boundedBuffer = boundedBuffer;
    }


    pthread_t coEditorThreads[3];
    for (int i = 0; i < 3; i++) {
        pthread_create(&coEditorThreads[i], NULL, CoEditor, &coEditorArgs[i]);
    }


    pthread_t Screen;
    pthread_create(&Screen,NULL,screenManager,boundedBuffer);






    for (int i = 0; i < numProducers; ++i) {
        pthread_join(producerThreads[i], NULL);
    }

    pthread_join(DispatcherThread, NULL);

   for (int i = 0; i < 3; i++) {
        pthread_join(coEditorThreads[i], NULL);
    }
    pthread_join(Screen,NULL);

   for (int i = 0; i < numProducers; ++i) {
        destroy_bounded_buffer(buffers[i]);
    }
    destroy_bounded_buffer(boundedBuffer);
    destroy_unbounded_buffer(coEditorArgs[0].unboundedBuffer);
    destroy_unbounded_buffer(coEditorArgs[1].unboundedBuffer);
    destroy_unbounded_buffer(coEditorArgs[2].unboundedBuffer);
    return 0;
}


void insert(Bounded_Buffer* bb, char* s) {
    sem_wait(&(bb->empty));
    pthread_mutex_lock(&(bb->mutex));
    bb->buffer[bb->nextInsertIndex] = strdup(s);


    bb->nextInsertIndex = (bb->nextInsertIndex + 1) % bb->length;
    bb->numofElementInserted = bb->numofElementInserted + 1;
    pthread_mutex_unlock(&(bb->mutex));
    sem_post(&(bb->full));

}



void* Producer(void* args) {
    argsForProducer* prodArguments = (argsForProducer*)args;
    Bounded_Buffer* bb = prodArguments->bb;

    while (1) {

        int type = rand() % 3;
        char typeStr[10];
        char str[100];

        switch (type) {
            case 0:
                strcpy(typeStr, "SPORTS");
                    sprintf(str, "Producer %d %s %d", prodArguments->numberOfProducts, typeStr, bb->numofSports);
                bb->numofSports++;
                break;
            case 1:
                strcpy(typeStr, "NEWS");
                sprintf(str, "Producer %d %s %d", prodArguments->numberOfProducts, typeStr, bb->numofNews);
                bb->numofNews++;
                break;
            default:
                strcpy(typeStr, "WEATHER");
                sprintf(str, "Producer %d %s %d", prodArguments->numberOfProducts, typeStr, bb->numofWeather);
                bb->numofWeather++;
                break;
        }

        insert(bb, str);
        if (bb->numofElementInserted ==prodArguments->numberOfElemnetNedded) {
            bb->flagForEndMessages=1;
            insert(bb,"DONE");

            pthread_exit(NULL);
        }
    }






}


char* remove_item(Bounded_Buffer* bb) {
    int value;
    sem_getvalue(&(bb->full), &value);
    if (value == 0){
        return "";
    }

    sem_wait(&(bb->full));
    pthread_mutex_lock(&(bb->mutex));
    char* item = bb->buffer[bb->nextDeleteIndex];
    bb->nextDeleteIndex = (bb->nextDeleteIndex + 1) % bb->length;
    bb->numOfElementRemoved=bb->numOfElementRemoved+1;
    pthread_mutex_unlock(&(bb->mutex));
    sem_post(&(bb->empty));
    return item;
}

void* Dispatcher(void* args) {
    argsForConsumer* consArguments = (argsForConsumer*)args;
    int numProducers = consArguments->numProducers;
    Bounded_Buffer** buffers = consArguments->buffers;
    Unbounded_Buffer* unboundedSport = consArguments->unboundedSport;
    Unbounded_Buffer* unboundedNews = consArguments->unboundedNews;
    Unbounded_Buffer* unboundedWeather = consArguments->unboundedWeather;

    int counter = 0; // Counter to keep track of the current buffer
    int ended = 0;

    while (ended != numProducers) {
        Bounded_Buffer* bb = buffers[counter];


        while (1) {

            char* item = remove_item(bb);

            if (strcmp(item, "") == 0)
                continue;

            if (strcmp(item, "DONE") == 0) {
                ended++;
                break;
            }

            // Extract the category from the item
            char category[10];
            extract_category(item, category);


            if (strcmp(category, "SPORTS") == 0) {
                insertToUnbounded(unboundedSport, item);
            } else if (strcmp(category, "NEWS") == 0) {
                insertToUnbounded(unboundedNews, item);
            } else {
                insertToUnbounded(unboundedWeather, item);
            }
        }

        counter = (counter + 1) % numProducers;
    }

    insertToUnbounded(unboundedSport,"DONE");
    insertToUnbounded(unboundedWeather,"DONE");
    insertToUnbounded(unboundedNews,"DONE");
    pthread_exit(NULL);
}









void *CoEditor(void *args) {
    argsForCoEditor *coEditor = (argsForCoEditor *) args;
    Bounded_Buffer *bb = coEditor->boundedBuffer;
    Unbounded_Buffer *ub = coEditor->unboundedBuffer;


    while (1) {
        char *item = removeFromUnBounded(ub);
        if (strcmp(item,"")==0)
            continue;
        if (strcmp(item,"DONE")==0) {
            break;
        }

        insert(bb,item);
    }
    insert(bb,"DONE");
    pthread_exit(NULL);
}


void* screenManager(void* args) {
    Bounded_Buffer* bb = (Bounded_Buffer*)args;



    int ended = 0;
    while (ended != 3) {
        char* item = remove_item(bb);
        if (strcmp(item,"")==0)
            continue;
        if (strcmp(item,"DONE")==0) {
            ended++;
            continue;
        }
        printf("%s\n", item);

    }
    printf("DONE\n");
    pthread_exit(NULL);
}

char* removeFromUnBounded(Unbounded_Buffer* ub) {
    int value;
    sem_getvalue(&(ub->sem_full), &value);
    if (value == 0){
        return "";
    }
    sem_wait(&(ub->sem_full));
    pthread_mutex_lock(&(ub->mutex));

    Node* nodeToRemove = ub->head;
    char* item = nodeToRemove->data;

    ub->head = ub->head->next;
    if (ub->head == NULL) {
        ub->tail = NULL;
    }

    ub->numOfElementRemoved++;

    pthread_mutex_unlock(&(ub->mutex));

    free(nodeToRemove);

    return item;
}
void insertToUnbounded(Unbounded_Buffer* ub, char* item) {
    Node* newNode = malloc(sizeof(Node));
    newNode->data = item;
    newNode->next = NULL;

    pthread_mutex_lock(&(ub->mutex));

    if (ub->head == NULL) {
        ub->head = newNode;
        ub->tail = newNode;
    } else {
        ub->tail->next = newNode;
        ub->tail = newNode;
    }

    ub->numofElementInserted++;

    pthread_mutex_unlock(&(ub->mutex));

    sem_post(&(ub->sem_full));
}

Unbounded_Buffer* create_unbounded_buffer() {
    Unbounded_Buffer* ub = malloc(sizeof(Unbounded_Buffer));
    ub->head = NULL;
    ub->tail = NULL;
    pthread_mutex_init(&(ub->mutex), NULL);
    sem_init(&(ub->sem_full), 0, 0); // Initialize the sem_full semaphore
    ub->numofElementInserted = 0;
    ub->numOfElementRemoved = 0;
    ub->flagForEndMessages = 0;
    return ub;
}

Bounded_Buffer* create_bounded_buffer(int size, int numberOfProducer) {
    Bounded_Buffer* bb = malloc(sizeof(Bounded_Buffer));
    bb->buffer = malloc(sizeof(char*) * (size));//need to
    bb->length = size;
    bb->nextInsertIndex = 0;
    bb->nextDeleteIndex = 0;
    sem_init(&(bb->empty), 0, size  );
    sem_init(&(bb->full), 0, 0);
    pthread_mutex_init(&(bb->mutex), NULL);
    bb->numberOfProducer = numberOfProducer;
    bb->numofSports = 0;
    bb->numofNews = 0;
    bb->numofWeather = 0;
    bb->numofElementInserted = 0;
    bb->numOfElementRemoved=0;
    bb->flagForEndMessages=0;
  return bb;
}



void extract_category(const char* item, char* category) {
    const char* delimiter = " ";
    const int expectedTokens = 3;

    char* copy = strdup(item);
    char* token = strtok(copy, delimiter);

    int tokenCount = 0;
    while (token != NULL && tokenCount < expectedTokens) {
        if (tokenCount == 2) {
            strcpy(category, token);
            break;
        }
        token = strtok(NULL, delimiter);
        tokenCount++;
    }

    free(copy);
}



void destroy_unbounded_buffer(Unbounded_Buffer* buffer) {
    Node* current = buffer->head;
    Node* next;


    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    pthread_mutex_destroy(&(buffer->mutex));
    sem_destroy(&(buffer->sem_full));

    free(buffer);
}


void destroy_bounded_buffer(Bounded_Buffer* bb) {
    for (int i = 0; i < bb->length; i++) {
        free(bb->buffer[i]);
    }
    free(bb->buffer);
    sem_destroy(&(bb->empty));
    sem_destroy(&(bb->full));
    pthread_mutex_destroy(&(bb->mutex));
    free(bb);
}