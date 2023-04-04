#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define mem_size 8
#define unit_size 50

typedef struct program {
    char name [20];
    int size ;
    int time ;
}prg;

typedef struct NODE {
    prg data;
    struct NODE *next;
}node;

typedef struct Queue {
    node *head;
    node *tail;
}Queue;

typedef struct memory {
    bool free;
    char prog [20];
    int start_adr;
    int length;
}memory;

typedef struct nodepart {
    memory partdata;
    struct noderpart *next;
}nodepart;

typedef struct Lis {
    nodepart *head;
}Lis;


int num_units (int size){
    return (size % unit_size == 0 ) ? (size / unit_size) : (size / unit_size +1 );
}

void print_bit_map (Lis *bit_map){
    printf("BIR MAP :\n");
    nodepart *current = bit_map->head ;
    while (current!=NULL){
        printf(" [%s][%d][%d] ",current->partdata.prog,current->partdata.start_adr,current->partdata.length);
        current=current->next;
    }
    printf("\n");
}

void addtoQ (Queue *q,prg p){
    node *n =(node *) malloc (sizeof (node));
    n->data = p;
    n->next = NULL;
    if (q->head == NULL){
        q->head = n;
    }else{
        q->tail->next = n;
    }
    q->tail = n;
}

prg deleteQ (Queue *q){
    node * head = q->head;
    prg p = head->data;

    q->head = head->next;
    if (q->head == NULL){
            q->tail = NULL;
    }
    free(head);
    return p;
}

void create_list_part (memory part, Lis *q){
    nodepart *n = (nodepart *) malloc (sizeof(nodepart));
    n->partdata = part;
    n->next = NULL;
    if (q->head == NULL){
        q->head = n;
    }else{
        nodepart  *current = q->head;
        while (current ->next != NULL){
            current = current->next;
        }
        current->next = n;
    }
}

void allocate_memory (Lis *bit_map,prg p){
    int num_units = (p.size+49)/50; //calculate number of allocation units required
    int min_length = num_units+1; //min len required for prg & the data

    //best fit algorithm
    nodepart *best_fit = NULL;
    int best_fit_length = mem_size+1;
    nodepart *curr = bit_map->head;
    while (curr != NULL){
        if (curr->partdata.free && curr ->partdata.length >=min_length){
            if (curr->partdata.length < best_fit_length){
                best_fit = curr;
                best_fit_length = curr->partdata.length;
            }
        }
        curr=curr->next;
    }
    if (best_fit != NULL){
            //change the data of allocated partition
        best_fit->partdata.free = false;
        strcpy(best_fit->partdata.prog,p.name);
        best_fit->partdata.length = num_units;
        //create hole if needed
        int hole_length = best_fit_length - min_length;
        if (hole_length>0){
            memory hole = {true , "hole",best_fit->partdata.start_adr+num_units,hole_length};
            create_list_part(hole,bit_map);
        }
        printf("Program %s allocated from address %d to %d\n", p.name, best_fit->partdata.start_adr, best_fit->partdata.start_adr + num_units - 1);
    }else{
        printf("Program %s cannot be allocated\n", p.name);
    }
}

void deallocate_memory (Lis *bit_map,char *name){
    //find mem part with program name
    nodepart *curr = bit_map->head;
    while (curr!=NULL){
        if (!curr->partdata.free && strcmp(curr->partdata.prog,name)==0){
            // change data of deallocated parts
            curr->partdata.free = true;
            strcpy(curr->partdata.prog,"hole");
            // njm3o holes li 7da b3dahom
            if (curr->next !=NULL && curr->next->partdata.free){
                curr->partdata.length += curr->next->partdata.length;
                curr->next = curr->next->next;
            }
            if (curr->partdata.start_adr>0){
                nodepart *prev = bit_map->head;
                while (prev->next != curr){
                    prev = prev->next;
                }
                if (prev->partdata.free){
                    prev->partdata.length += curr->partdata.length;
                    prev->next = curr->next;
                    free (curr);
                    curr  = prev ;
                }
            }
            printf("program %s deallocated \n",name);
            return;
        }
        curr = curr->next;
    }
    printf("program %s not found\n", name);
}
