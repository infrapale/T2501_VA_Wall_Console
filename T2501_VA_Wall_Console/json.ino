/**
 * @brief Build light control json message
 * @param p  = pointer to return value (char array)
 * @param max_len
 * @param zone char pointer
 * @param addr char pointer
 * @param value char pointer
 * @param remark char pointer
 * @retval *p  = pointer to return value (char array)
 * @retval char array len
 */
uint8_t json_char_array(char *p, const uint8_t max_len, const char *zone, const char *addr, const char *value, const char *remark) {
     if (max_len >= strlen(zone)+strlen(addr)+strlen(value)+strlen(remark)){
       strcpy(p,"{\"Z\":\"");
       strcat(p, zone);
       strcat(p,"\",");
       strcat(p,"\"S\":\"");
       strcat(p,addr);
       strcat(p, "\",");
       strcat(p,"\"V\":\"");
       strcat(p,value);
       strcat(p,"\",");
       strcat(p,"\"R\":\"");
       strcat(p,remark);
       strcat(p,"\"}");
       return strlen(p);
     }
     else
     {
       return 0;
     }
  
}
