#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

int main(void) {
    CURL *easy_handle;
    CURLM *multi_handle;
    CURLMcode mcode;
    int still_running = 1;

    const char *url = "https://example.com/file.txt";
    const char *output_filename = "downloaded_file.txt";
    
    FILE *fp = fopen(output_filename, "wb");
    if (!fp) {
        fprintf(stderr, "Failed to open file for writing\n");
        return 1;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    easy_handle = curl_easy_init();

    curl_easy_setopt(easy_handle, CURLOPT_URL, url);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1L);

    multi_handle = curl_multi_init();
    curl_multi_add_handle(multi_handle, easy_handle);

    
    while (still_running) {
        printf("\nmulti perform");
        curl_multi_perform(multi_handle, &still_running);
        int numfds;
        printf("\ncurl_multi_wait started");
        mcode = curl_multi_wait(multi_handle, NULL, 0, 10000, &numfds);
        printf("\ncurl_multi_wait finished");
        if (mcode != CURLM_OK) {
            fprintf(stderr, "curl_multi_wait() failed: %s\n", curl_multi_strerror(mcode));
            break;
        }
    }

    // Check for any errors
    CURLMsg *msg;
    int msgs_left;
    while ((msg = curl_multi_info_read(multi_handle, &msgs_left))) {
        if (msg->msg == CURLMSG_DONE) {
            if (msg->data.result != CURLE_OK) {
                fprintf(stderr, "Download failed: %s\n", curl_easy_strerror(msg->data.result));
            } else {
                printf("Download completed successfully.\n");
            }
        }
    }

    curl_multi_remove_handle(multi_handle, easy_handle);
    curl_easy_cleanup(easy_handle);
    curl_multi_cleanup(multi_handle);
    curl_global_cleanup();
    fclose(fp);

    return 0;
}
