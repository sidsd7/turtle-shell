#ifndef AI_INTEGRATION_H
#define AI_INTEGRATION_H

#include <curl/curl.h>
#include <json-c/json.h>

// Structure to hold response data
typedef struct {
    char *data;
    size_t size;
} AIResponse;

// Function declarations
char* natural_language_to_command(const char* input);
char* validate_command(const char* command);
void init_ai_integration(void);
void cleanup_ai_integration(void);

#endif 