#pragma once

#define ERROR_PRINT(msg) \
    do{         \
        printf("%s\n", msg); \
    }while(0);

#define ERROR_CHECK(fDir, ret, msg) \
    if ((fDir) == ret) { \
        std::cerr << "Error: " << msg << std::endl; \
        return; \
    }

/*#define ERROR_PRINT(fmsg, msg) \
    do { \
        printf("fmsg\n", msg); \
        } while (0);
*/