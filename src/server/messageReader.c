#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <fnmatch.h>
#include "utils.c"
#include "hardware.c"
#include "DM.c"
#include "network.c"
#include <json.h>


//every machine tied to the server
network AllDMs;
void init_all(){
  init_network(&AllDMs);
}

char* get_json_attr_object_server(char* attr, struct json_object *json){
  struct json_object  *tmp;
  struct json_object *tmp2;
  json_object_object_get_ex(json, "object", &tmp);
  tmp2 = json_tokener_parse(json_object_to_json_string_ext(tmp, JSON_C_TO_STRING_PRETTY));
  json_object_object_get_ex(tmp, attr, &tmp2);
  char* attr_got = malloc(sizeof(char)*strlen(json_object_to_json_string_ext(tmp2, JSON_C_TO_STRING_PRETTY)));
  char* temp_attr =  malloc(sizeof(char)*strlen(json_object_to_json_string_ext(tmp2, JSON_C_TO_STRING_PRETTY)));
  strcpy(temp_attr, json_object_to_json_string_ext(tmp2, JSON_C_TO_STRING_PRETTY));
  strcpy(attr_got, temp_attr);
  free(temp_attr);
  return attr_got;
}

char* get_json_attr_server(char* attr, struct json_object *json){
  struct json_object  *tmp;
  json_object_object_get_ex(json, attr, &tmp);
  char* attr_got = malloc(sizeof(char)*strlen(json_object_to_json_string_ext(tmp, JSON_C_TO_STRING_PRETTY))+1);
  char* temp_attr =  malloc(sizeof(char)*strlen(json_object_to_json_string_ext(tmp, JSON_C_TO_STRING_PRETTY))+1);
  strcpy(temp_attr, json_object_to_json_string_ext(tmp, JSON_C_TO_STRING_PRETTY));
  strcpy(attr_got, temp_attr);
  free(temp_attr);
  free(tmp);
  return attr_got;
}
//parsing json to readable objects for the server boys
void parseJson(char* args){
  struct json_object *jobj;
  hardware thing;
  init_hardware(&thing);
  struct {
		int flag;
		const char *flag_str;
	} json_flags[] = {
		{ JSON_C_TO_STRING_PLAIN, "JSON_C_TO_STRING_PLAIN" },
		{ JSON_C_TO_STRING_SPACED, "JSON_C_TO_STRING_SPACED" },
		{ JSON_C_TO_STRING_PRETTY, "JSON_C_TO_STRING_PRETTY" },
		{ JSON_C_TO_STRING_NOZERO, "JSON_C_TO_STRING_NOZERO" },
		{ JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY, "JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY" },
		{ -1, NULL }
};
  jobj = json_tokener_parse(args);
  char *ip = get_json_attr_server("ip", jobj);
  char *subnet_mask = get_json_attr_server("sub", jobj);
  char* op = get_json_attr_server("op", jobj);
  if(strcmp(op,"\"add\"") == 0){
    set_object_dim(&thing, get_json_attr_object_server("dim", jobj));
    set_object_color(&thing, get_json_attr_object_server("color",jobj));
    set_object_name(&thing, get_json_attr_object_server("name",jobj));
    set_object_state(&thing, get_json_attr_object_server ("state",jobj));
    set_object_type(&thing, get_json_attr_object_server("type",jobj));
    if(strcmp(get_DM_IP(&AllDMs, ip)->ip, "none") != 0){
      DM tmp = *get_DM_IP(&AllDMs,ip);
      add_to_DM(&tmp, &thing);
      update_DM_on_net(&tmp, ip, &AllDMs);
    }else{
      DM tmpDM;
      init_DM(&tmpDM);
      tmpDM.size = 0;
      create_DM(&tmpDM, ip, subnet_mask);
      tmpDM = *add_to_DM(&tmpDM, &thing);
      add_DM_to_net(tmpDM, &AllDMs);
    }
  }
  if(strcmp(op,"delete") == 0){
    set_object_dim(&thing, get_json_attr_object_server("dim", jobj));
    set_object_color(&thing, get_json_attr_object_server("color",jobj));
    set_object_name(&thing, get_json_attr_object_server("name",jobj));
    set_object_state(&thing, get_json_attr_object_server ("state",jobj));
    if(strcmp(get_DM_IP(&AllDMs, ip)->ip, "none") != 0){
      DM tmpDM = *get_DM_IP(&AllDMs, ip);
      remove_from_dm(&tmpDM,&thing);
    }//else{

    //}
  }

  free(ip);
  free(subnet_mask);
  free(op);
  free(jobj);

  for(int i = 0; i < AllDMs.size; i++){
    printf("DM %s exists on network.\n", AllDMs.things[i].ip);
    for(int j = 0; j < AllDMs.things[i].size; j++){
      printf("\t Hardware %s exists on DM.\n", AllDMs.things[i].objects[j].name);
    }
  }




}
