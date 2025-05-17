#include "../include/ai_integration.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <ctype.h>  // For isspace()

#define OLLAMA_URL "http://localhost:11434/api/generate"
#define MAX_RESPONSE_SIZE 4096
#define USE_FALLBACK_MODE 0
#ifndef MAX_LINE
#define MAX_LINE 1024
#endif

static CURL *curl;
static struct curl_slist *headers;

// Don't redefine AIResponse - use the one from the header
// Remove or comment out this structure definition
/* 
typedef struct {
    char *data;
    size_t size;
} AIResponse;
*/

// Callback function to handle API response
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    AIResponse *resp = (AIResponse *)userp;
    
    char *ptr = realloc(resp->data, resp->size + realsize + 1);
    if(!ptr) {
        fprintf(stderr, "Out of memory!\n");
        return 0;
    }
    
    resp->data = ptr;
    memcpy(&(resp->data[resp->size]), contents, realsize);
    resp->size += realsize;
    resp->data[resp->size] = 0;
    
    return realsize;
}

void init_ai_integration(void) {
    // Initialize curl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    
    // Set up headers
    headers = curl_slist_append(NULL, "Content-Type: application/json");
    
    printf("AI integration initialized.\n");
}

void cleanup_ai_integration(void) {
    if (headers) curl_slist_free_all(headers);
    if (curl) curl_easy_cleanup(curl);
    curl_global_cleanup();
}

static char* make_ollama_request(const char* prompt, const char* system_message) {
    if (!curl || !headers) {
        return NULL;
    }
    
    // Prepare JSON payload
    struct json_object *root = json_object_new_object();
    json_object_object_add(root, "model", json_object_new_string("phi"));
    
    // Combine system message and prompt
    char full_prompt[4096];
    snprintf(full_prompt, sizeof(full_prompt), 
             "System: %s\nUser: %s", 
             system_message, prompt);
    
    json_object_object_add(root, "prompt", json_object_new_string(full_prompt));
    const char *json_str = json_object_to_json_string(root);
    
    AIResponse response = {0};
    response.data = malloc(1);
    response.size = 0;
    
    curl_easy_setopt(curl, CURLOPT_URL, OLLAMA_URL);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    
    CURLcode res = curl_easy_perform(curl);
    
    json_object_put(root);  // Free JSON objects
    
    if (res != CURLE_OK) {
        free(response.data);
        return NULL;
    }
    
    // Fix the response parsing
    char *full_response = malloc(MAX_RESPONSE_SIZE);
    if (!full_response) {
        free(response.data);
        return NULL;
    }
    full_response[0] = '\0';  // Initialize as empty string
    
    char *line = strtok(response.data, "\n");
    while (line != NULL) {
        struct json_object *resp_json = json_tokener_parse(line);
        if (resp_json) {
            struct json_object *response_text;
            if (json_object_object_get_ex(resp_json, "response", &response_text)) {
                const char *text = json_object_get_string(response_text);
                if (text) {
                    strcat(full_response, text);
                }
            }
            json_object_put(resp_json);
        }
        line = strtok(NULL, "\n");
    }
    
    free(response.data);
    
    // Trim any whitespace from the beginning and end
    char *start = full_response;
    while (*start && isspace(*start)) start++;
    
    char *end = start + strlen(start) - 1;
    while (end > start && isspace(*end)) *end-- = '\0';
    
    if (start != full_response) {
        memmove(full_response, start, strlen(start) + 1);
    }
    
    return full_response;
}

char* natural_language_to_command(const char* input) {
    if (USE_FALLBACK_MODE) {
        printf("Using fallback mode for translation\n");
        // Simple keyword mapping
        if (strstr(input, "list") || strstr(input, "show") || strstr(input, "display") || 
            strstr(input, "files") || strstr(input, "directory")) {
            if (strstr(input, "detail") || strstr(input, "long")) {
                return strdup("custom_ls_long");
            } else {
                return strdup("custom_ls");
            }
        } else if (strstr(input, "current") || strstr(input, "where")) {
            return strdup("custom_pwd");
        } else if (strstr(input, "help")) {
            return strdup("custom_help");
        } else {
            return strdup("custom_help");
        }
    } else {
        const char* system_msg = "You are a command translator bot. DO NOT be conversational. DO NOT ask questions.\n"
                               "ONLY output the exact shell command, nothing else.\n"
                               "The only valid commands start with custom_. For example:\n"
                               "- custom_ls (to list files)\n"
                               "- custom_cat file.txt (to show file contents)\n"
                               "- custom_mkdir new_dir (to create directory)\n"
                               "- custom_rm file.txt (to delete file)\n"
                               "- custom_cd dir_name (to change directory)\n"
                               "- custom_pwd (to show current directory)\n"
                               "- custom_echo text (to display text)\n"
                               "- custom_ls_long (to list files with details)\n"
                               "- custom_touch filename (to create empty file)\n"
                               "If you don't understand, just output 'custom_help'";
        char* result = make_ollama_request(input, system_msg);
        
        // If the response doesn't start with "custom_", try to fix it
        if (result && strncmp(result, "custom_", 7) != 0) {
            // This doesn't look like a command, return a simple fallback
            free(result);
            
            // Try to extract keywords and map to commands
            if (strstr(input, "list") || strstr(input, "show") || strstr(input, "display") || 
                strstr(input, "files") || strstr(input, "directory")) {
                if (strstr(input, "detail") || strstr(input, "long")) {
                    return strdup("custom_ls_long");
                } else {
                    return strdup("custom_ls");
                }
            } else if (strstr(input, "current") || strstr(input, "where")) {
                return strdup("custom_pwd");
            } else if (strstr(input, "help")) {
                return strdup("custom_help");
            } else {
                // Fallback to help
                return strdup("custom_help");
            }
        } else if (result) {
            // Extra verification - check if it's actually a valid command
            const char *valid_commands[] = {
                "custom_ls", "custom_ls_long", "custom_cat", "custom_cp",
                "custom_mkdir", "custom_rmdir", "custom_rm", "custom_echo",
                "custom_touch", "custom_cd", "custom_pwd", "custom_history",
                "custom_alias", "custom_unalias", "custom_help", "custom_exit",
                NULL
            };
            
            // Extract the base command
            char cmd_copy[MAX_LINE];
            strncpy(cmd_copy, result, MAX_LINE-1);
            cmd_copy[MAX_LINE-1] = '\0';
            
            char *space_pos = strchr(cmd_copy, ' ');
            if (space_pos) *space_pos = '\0';
            
            // Check if it's valid
            int valid = 0;
            for (int i = 0; valid_commands[i] != NULL; i++) {
                if (strcmp(cmd_copy, valid_commands[i]) == 0) {
                    valid = 1;
                    break;
                }
            }
            
            if (!valid) {
                free(result);
                return strdup("custom_help");
            }
        }
        
        return result;
    }
}

char* validate_command(const char* command) {
    // First, let's do our own quick validation for invalid commands
    const char *valid_commands[] = {
        "custom_ls", "custom_ls_long", "custom_cat", "custom_cp",
        "custom_mkdir", "custom_rmdir", "custom_rm", "custom_echo",
        "custom_touch", "custom_cd", "custom_pwd", "custom_history",
        "custom_alias", "custom_unalias", "custom_help", "custom_exit",
        NULL
    };
    
    // Extract the base command (before any arguments)
    char cmd_copy[MAX_LINE];
    strncpy(cmd_copy, command, MAX_LINE-1);
    cmd_copy[MAX_LINE-1] = '\0';
    
    char *space_pos = strchr(cmd_copy, ' ');
    if (space_pos) *space_pos = '\0';
    
    // Check if it's a valid command
    int valid = 0;
    for (int i = 0; valid_commands[i] != NULL; i++) {
        if (strcmp(cmd_copy, valid_commands[i]) == 0) {
            valid = 1;
            break;
        }
    }
    
    if (!valid) {
        return strdup("Unknown command. Use 'custom_help' to see available commands.");
    }
    
    // Then use AI for deeper validation of valid commands
    const char* system_msg = "You are a shell command validator. Your job is to check if commands are safe.\n\n"
                           "IMPORTANT RULES:\n"
                           "1. If the command is completely safe, respond ONLY with 'Command is safe.'\n"
                           "2. If the command involves ANY file or directory deletion (rm, rmdir), warn about data loss\n"
                           "3. Keep warnings brief and specific to the actual risk\n"
                           "4. NEVER be conversational or ask questions\n"
                           "5. Only valid commands start with 'custom_'\n\n"
                           "Examples of unsafe commands:\n"
                           "- custom_rm (warn about file deletion)\n"
                           "- custom_rmdir (warn about directory deletion)\n\n"
                           "ALL other commands should be considered safe.";
    
    char* result = make_ollama_request(command, system_msg);
    
    // Improved fallback logic
    if (result) {
        // Check if the response is neither a clear safety message nor a warning
        if (strstr(result, "Command is safe") == NULL && 
            strstr(result, "Warning") == NULL && 
            strstr(result, "unsafe") == NULL && 
            strstr(result, "danger") == NULL &&
            strstr(result, "delete") == NULL &&
            strstr(result, "remove") == NULL) {
            
            // The AI gave an unpredictable response, use our own validation
            free(result);
            
            // Simple rule-based validation
            if (strstr(command, "custom_rm")) {
                return strdup("Warning: This command will delete files permanently.");
            } else if (strstr(command, "custom_rmdir")) {
                return strdup("Warning: This command will delete directories.");
            } else {
                return strdup("Command is safe.");
            }
        }
        
        // If the response is extremely long, truncate it
        if (strlen(result) > 100) {
            result[97] = '.';
            result[98] = '.';
            result[99] = '.';
            result[100] = '\0';
        }
    } else {
        // If we got NULL from the AI, use our fallback
        if (strstr(command, "custom_rm")) {
            return strdup("Warning: This command will delete files permanently.");
        } else if (strstr(command, "custom_rmdir")) {
            return strdup("Warning: This command will delete directories.");
        } else {
            return strdup("Command is safe.");
        }
    }
    
    return result;
} 