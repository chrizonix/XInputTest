//-------------------------------
// Includes
//-------------------------------
#include <windows.h>

#include <stdlib.h>
#include <iostream>

#include <vector>
#include <stdio.h>

#include <chrono>
#include <ctime>

#include "qsp.h"
#include "iqsort.h"

#include "CXBOXController.h"

//-------------------------------
// Namespaces
//-------------------------------
using namespace std;

//-------------------------------
// Defines
//-------------------------------
#define MAX_SAMPLES 1000
#define THRESHOLD   1.5

//-------------------------------
// Prototypes
//-------------------------------
void rapid_outlier_detection(double *values, double *scores, double avg, string &summary);
void get_time_string(char *buffer, size_t size);
void write_report(double *values, double *scores, double avg, string &summary);

//-------------------------------
// Functions
//-------------------------------
int main(int argc, char* argv[]) {
    bool using_higher_res_timer = false;

    // Set Timer Resolution to 1 ms
    if (timeBeginPeriod(1) != TIMERR_NOCANDO) {
        using_higher_res_timer = true;
    }

    // Create Controller
    CXBOXController* controller = new CXBOXController(1);

    // Check Connection
    if (controller->IsConnected()) {
        cout << "Ready! Waiting for " << MAX_SAMPLES << " Samples (Move Left Stick)" << endl;
    }

    // Previous Stick Position
    SHORT LX = 0;
    SHORT LY = 0;

    // Sample Index
    int index = 0;

    // Store Samples and Outlier Score
    double* values = new double[MAX_SAMPLES];
    double* scores = new double[MAX_SAMPLES];

    // Query Performance Counter
    LARGE_INTEGER frequency;                // Ticks per Second
    LARGE_INTEGER time_start, time_now;     // Ticks

    // Elapsed Time
    char elapsed_text[32];
    double elapsed_time = 0;

    // Get Ticks per Second
    QueryPerformanceFrequency(&frequency);

    //-------------------
    // Collect Samples
    //-------------------
    while (true) {
        if (controller->IsConnected()) {
            XINPUT_STATE state = controller->GetState();

            // Check Left Stick Position
            if (state.Gamepad.sThumbLX != LX || state.Gamepad.sThumbLY != LY) {
                // Check Idle
                if (LX == 0 && LY == 0) {
                    // Don't Count Initial Movement (Out of Center)
                    QueryPerformanceCounter(&time_start);
                } else {
                    // Get Current Time
                    QueryPerformanceCounter(&time_now);

                    // Calculate Elapsed Time
                    elapsed_time = (time_now.QuadPart - time_start.QuadPart) * 1000.0 / frequency.QuadPart;

                    // Print Duration (cout seems to be faster than printf)
                    sprintf_s(elapsed_text, "Duration %.2f ms", elapsed_time);
                    cout << elapsed_text << endl;

                    // Add Sample and Break Loop at the End
                    if (index < MAX_SAMPLES) {
                        values[index++] = elapsed_time;
                    } else {
                        break;
                    }

                    // Get Current Time
                    QueryPerformanceCounter(&time_start);
                }
            }

            // Get Left Stick Values
            LX = state.Gamepad.sThumbLX;
            LY = state.Gamepad.sThumbLY;
        } else {
            // Controller Not Found
            cout << "Error! Player 1 - XBOX 360 Controller Not Found!\n";
            cout << "Press Enter Key To Exit";
            cin.get();

            // Exit Failure
            exit(EXIT_FAILURE);
        }
    }

    // Reset Timer Resolution
    if (using_higher_res_timer) {
        timeEndPeriod(1);
    }

    //-------------------
    // Statistics
    //-------------------
    double sum = 0;
    double max_value = 0;
    double min_value = 1000;

    double jitter = 0;
    double transit = 0;

    double last_transit = 0;
    double delta_transit = 0;

    // Aggregate Values
    for (int i = 0; i < MAX_SAMPLES; ++i) {
        // Sum for Average Value
        sum += values[i];

        // Min / Max Values
        min_value = min(min_value, values[i]);
        max_value = max(max_value, values[i]);

        // Jitter Calculation from RFC 1889, Real Time Protocol (RTP)
        // J = J + ( | D(i-1,i) | - J ) / 16
        transit = values[i];

        if (last_transit != 0.0) {
            delta_transit = fabs(transit - last_transit);
            jitter += (delta_transit - jitter) / (16.0);
        }

        last_transit = transit;
    }

    // Calculate Average Polling Rate
    double avg = (sum / (double) MAX_SAMPLES);

    // Summary Block
    string summary;
    char line[64];

    // Detect and Print Outliers
    rapid_outlier_detection(values, scores, avg, summary);

    // Write Statistics
    sprintf_s(line, "\nMin %f ms - Max %f ms - Jitter %f ms\n", min_value, max_value, jitter);
    summary.append(line);

    sprintf_s(line, "Average %f ms - Polling Rate %f Hz\n", avg, 1000 / avg);
    summary.append(line);

    // Print Statistics
    cout << endl << summary.c_str() << endl;

    // Wait for Input
    cout << "Write Report To File? J/[N]: ";

    // Get Answer
    char answer = cin.get();

    // Check Answer
    if (answer == 'j' || answer == 'J') {
        // Write Report To File
        write_report(values, scores, avg, summary);
    }

    // Free Memory
    delete(values);
    delete(scores);
    delete(controller);

    // Return Status
    return EXIT_SUCCESS;
}

void rapid_outlier_detection(double *values, double *scores, double avg, string &summary) {
    long i, n = 0, d = 0, seed = 0, n_sample = 20;
    int k = 10, norm = 1, sort = 1;
    char line[64];

    // n: # of rows, d: # of columns
    n = MAX_SAMPLES; d = 1;

    // Copy Values Array (Because of Data Manipulation)
    double *normalized = (double *) malloc(sizeof(double) * n);
    memcpy_s(normalized, sizeof(double) * n, values, sizeof(double) * n);

    // Normalization
    normalize(normalized, n, d);

    // Compute QSP Score
    qsp(normalized, n, d, n_sample, seed, scores);

    // Outliers
    int outliers = 0;

    // Count Outliers
    for (i = 0; i < n; i++) {
        if (scores[i] > 1.0 && (values[i] - avg) > THRESHOLD) {
            outliers++;
        }
    }

    // Print Outliers
    sprintf_s(line, "Outliers: %d / %d\n", outliers, MAX_SAMPLES);
    summary.append(line);

    // Index QSort for Ranking
    if (sort == 1 && outliers > 0) {
        long *index = (long *) malloc(sizeof(long) * n);

        // Fill Indices
        for (i = 0; i < n; i++) {
            index[i] = i;
        }

        // Index QSort for Ranking
        iqsort(index, scores, n);

        // Top "x" Outliers
        k = min(outliers, k);

        // Print Top "x" Outliers
        sprintf_s(line, "Top %d Outliers: ", k);
        summary.append(line);

        for (i = 0; i < (k - 1); i++) {
            sprintf_s(line, "%.2f ms, ", values[index[i]]);
            summary.append(line);
        }

        sprintf_s(line, "%.2f ms\n", values[index[i]]);
        summary.append(line);

        // Free Memory
        free(index);
    }

    // Free Memory
    free(normalized);
}

void get_time_string(char *buffer, size_t size) {
    time_t rawtime;
    struct tm timeinfo;

    // Get Local Time
    time(&rawtime);
    localtime_s(&timeinfo, &rawtime);

    // Format Local Time
    strftime(buffer, size, "%Y%m%d_%H%M%S", &timeinfo);
}

void write_report(double *values, double *scores, double avg, string &summary) {
    // File Name Variables
    char date_time[16];
    char file_name[32];

    // Generate Report File Name
    get_time_string(date_time, sizeof(date_time));
    sprintf_s(file_name, "Report_%s.txt", date_time);

    // File Handle
    FILE *fp = NULL;

    // Open File
    if (fopen_s(&fp, file_name, "w") == 0) {
        // Write Summary
        fprintf(fp, "%s\n", summary);

        // Write Header
        fprintf(fp, "--------------------\n");
        fprintf(fp, " Outlier | Duration \n");
        fprintf(fp, "--------------------\n");

        // Write to File
        for (int i = 0; i < MAX_SAMPLES; i++) {
            fprintf(fp, "   %3s   | %5.2f ms \n", (scores[i] > 1.0 && (values[i] - avg) > THRESHOLD) ? "==>" : "   ", values[i]);
        }

        // Close File
        fclose(fp);
    }
}
