#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <telebot.h>

#define SIZE_OF_ARRAY(array) (sizeof(array) / sizeof(array[0]))

int main(int argc, char *argv[])
{
    // Take the value of CPU --------
    FILE *fp1;
    char cpu_value[1024];
    char mem_value[1024];

    // Open the /proc/stat file for reading
    fp1 = fopen("/root/output.txt", "r");
    if (fp1 == NULL) {
        perror("Error opening /proc/stat");
        exit(1);
    }

    // Read the first line which contains overall CPU statistics
    fgets(cpu_value, sizeof(cpu_value), fp1);
    fgets(mem_value, sizeof(mem_value), fp1);

    // Close the file
    fclose(fp1);

    // --------------------------------

    printf("Welcome to Cymbal-monkey\n");

    FILE *fp = fopen(".token", "r");
    if (fp == NULL)
    {
        printf("Failed to open .token file\n");
        return -1;
    }

    char token[1024];
    if (fscanf(fp, "%s", token) == 0)
    {
        printf("Failed to read token\n");
        fclose(fp);
        return -1;
    }
    printf("Token: %s\n", token);
    fclose(fp);

    telebot_handler_t handle;
    if (telebot_create(&handle, token) != TELEBOT_ERROR_NONE)
    {
        printf("Telebot create failed\n");
        return -1;
    }

    telebot_user_t me;
    if (telebot_get_me(handle, &me) != TELEBOT_ERROR_NONE)
    {
        printf("Failed to get bot information\n");
        telebot_destroy(handle);
        return -1;
    }

    printf("ID: %d\n", me.id);
    printf("First Name: %s\n", me.first_name);
    printf("User Name: %s\n", me.username);

    telebot_put_me(&me);

    int index, count, offset = -1;
    telebot_error_e ret;
    telebot_message_t message;
    telebot_update_type_e update_types[] = {TELEBOT_UPDATE_TYPE_MESSAGE};

    while (1)
    {
        telebot_update_t *updates;
        ret = telebot_get_updates(handle, offset, 20, 0, update_types, 0, &updates, &count);
        if (ret != TELEBOT_ERROR_NONE)
            continue;
        printf("Number of updates: %d\n", count);
        for (index = 0; index < count; index++)
        {
            message = updates[index].message;
            if (message.text)
            {
                printf("%s: %s \n", message.from->first_name, message.text);
                char str[4096];
	        if (strstr(message.text, "/cpu"))
                {
                    snprintf(str, SIZE_OF_ARRAY(str), "<i>%s</i>", cpu_value);
                }
        	else if (strstr(message.text, "/memory"))
                {
                    snprintf(str, SIZE_OF_ARRAY(str), "<i>%s</i>", mem_value);
                }
                else if (strstr(message.text, "/values")){
	            snprintf(str, SIZE_OF_ARRAY(str), "<i>%s\n%s</i>", cpu_value, mem_value);
		}
		else {
                    snprintf(str, SIZE_OF_ARRAY(str), "Hello %s, cymbal-monkey currently offers the following options: \n/help \n/cpu \n/memory \n/values", message.from->first_name);
		}
                ret = telebot_send_message(handle, message.chat->id, str, "HTML", false, false, updates[index].message.message_id, "");
                if (ret != TELEBOT_ERROR_NONE)
                {
                    printf("Failed to send message: %d \n", ret);
                }
            }
            offset = updates[index].update_id + 1;
        }
        telebot_put_updates(updates, count);

        sleep(1);
    }

    telebot_destroy(handle);

    return 0;
}
