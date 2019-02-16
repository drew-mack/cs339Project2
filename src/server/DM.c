#include <stdio.h>
#include <stdlib.h>


typedef struct DM{
  hardware* objects;
  char* ip;
  char* subnet_mask;
  int size;
}DM;

void init_DM(DM* tempDM){
  tempDM = malloc(sizeof(DM));
  tempDM->objects = malloc(sizeof(hardware));
  tempDM->size = 0;
}

void create_DM(DM* dungeonMaster, char* ip, char* subnet){
  dungeonMaster->ip = malloc(sizeof(char)*strlen(ip));
  strcpy(dungeonMaster->ip, ip);
  dungeonMaster->subnet_mask = malloc(sizeof(char)*strlen(subnet));
  strcpy(dungeonMaster->subnet_mask, subnet);
  //dungeonMaster->objects = malloc(sizeof(hardware));
}

DM* create_nullDM(){
  DM* nullDM = malloc(sizeof(DM));
  nullDM->ip = malloc(sizeof(char)*5);
  strcpy(nullDM->ip, "none");
  return nullDM;
}

int name_check(DM* tempDM, char* name){
  for(int i = 0; i < tempDM->size; i++){
    if(strcmp(tempDM->objects[i].name, name) == 0){
      return 0;
    }
  }
  return 1;
}

DM* add_to_DM(DM* requested_DM, hardware* newWare){
  if(strcmp(requested_DM->ip, "none") == 0){
    return create_nullDM();
  }else{
    printf("%d\n", requested_DM->size);
    if(requested_DM->size == 0){
      printf("here\n");
      requested_DM->objects = malloc(sizeof(hardware)*1);
      hardware_copy_new(&requested_DM->objects[0], newWare);
      requested_DM->size+=1;
      return requested_DM;
    }else{
      requested_DM->objects = realloc(requested_DM->objects, sizeof(hardware)*(requested_DM->size+1));
      hardware_copy_new(&requested_DM->objects[requested_DM->size], newWare);
      requested_DM->size+=1;
      return requested_DM;
    }
  }
}

hardware* get_hardware_name(DM* tmpDM, char* name){
  for(int i =0; i < tmpDM->size; i++){
    if(strcmp(name, tmpDM->objects[i].name) == 0){
      return &tmpDM->objects[i];
    }
  }
  return create_nullDM();
}

DM* remove_from_dm(DM* tmpDM, hardware* badware){
  DM *newDM = malloc(sizeof(hardware));
  newDM->objects = malloc(tmpDM->size * sizeof(hardware));
  newDM->size = tmpDM->size;
  newDM->ip = malloc(sizeof(char)*strlen(tmpDM->ip)+1);
  strcpy(newDM->ip, tmpDM->ip);
  newDM->subnet_mask = malloc(sizeof(char)*strlen(tmpDM->subnet_mask)+1);
  strcpy(newDM->subnet_mask, tmpDM->subnet_mask);
  int dmPos=0;
  for(int i = 0; i < tmpDM->size; i++){
    if(strcmp(badware->name, tmpDM->objects[i].name) != 0){
      hardware_copy_new(&newDM->objects[dmPos], &tmpDM->objects[i]);
      dmPos+=1;
    }
  }
  return newDM;
}



void freeDM(DM* dm){
  for (int i =0; i < dm->size; i++){
    free_hardware(&dm->objects[i]);
  }
  free(dm->ip);
  free(dm->subnet_mask);
}
