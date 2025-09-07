#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <unordered_map>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>     // for fork, execlp, usleep, sleep
#include <sys/types.h>  // for pid_t
#include <signal.h>     // for kill
#endif

using namespace std;

unordered_map<long long, long long> divSumMap;
long long range = 10000000001;

long long divisorSum(long long num) {
    long long sum = 1;
    for (long long i = 2; i <= sqrt(num); i++) {
        if (num % i == 0) {
            sum += i;
            if (i * i != num) {
                sum += num / i;
            }
        }
    }
    divSumMap[num] = sum;
    return sum;
}

long long divisorSumCached(long long num) {
    if (divSumMap.count(num)) {
        return divSumMap[num];
    }
    return divisorSum(num);
}

string getInteresting(long long priorityLevel, long long num, long long divisorSum) {
    if (num == divisorSum && priorityLevel > 0) {
        return to_string(num) + " IS PERFECT!!!" + "\n";
    } else if (divisorSum == 1 && priorityLevel > 1) {
        return to_string(num) + " is prime!" + "\n";
    } else {
        return "";
    }
}

long long sociabletf(long long depth, long long num) {
    long long running = num;
    for (int i = 0; i < depth; i++) {
        running = divisorSumCached(running);
        if (running == num) {
            return i + 1;
        } else if (running > (range^2)) {
            return -1;
        }
    }
    return 0;
}

string sociable(long long depth, long long num) {
    long long sNumber = sociabletf(depth + 32, num);
    if (sNumber > 2 && sNumber < 28) {
        return to_string(num) + " is sociable with depth " + to_string(sNumber) + "." + "\n";
    } else if (sNumber > 28) {
        return to_string(num) + " IS SOCIABLE WITH DEPTH " + to_string(sNumber) + "!!!!!!" + "\n";
    } else {
        return "";
    }
}

void check(long long start, long long end, int depth, ofstream numsFile) {
    for (long long i = start; i < end; i++) {
        if (i % 100000 == 0) {
            cout << i << endl;
        }
        numsFile << sociable(depth, i);
        numsFile.flush();
    }
}

int main() {
    ofstream numsFile("~/Downloads/nums.txt");
    long long priority = 1;
    long long divsum = 0;

#ifdef _WIN32
    // --- Windows: PowerShell ---
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    const char* cmd = "powershell.exe -Command \"Get-Content nums.txt -Wait\"";

    if (!CreateProcess(
            NULL,
            (LPSTR)cmd,
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si,
            &pi)) {
        cerr << "Failed to start PowerShell!" << endl;
        return 1;
    }

    Sleep(500);

    check(2, range, 32, std::move(numsFile));

    ifstream iNumsFile("nums.txt");

    TerminateProcess(pi.hProcess, 0);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    Sleep(2000);

#else
    // --- macOS/Linux: tail -f ---
    pid_t pid = fork();
    if (pid == 0) {
        execlp("osascript", "osascript", "-e",
               "tell app \"Terminal\" to do script \"tail -f /Users/juderasmussen/Downloads/nums.txt\"",
               (char*)NULL);
        perror("execlp failed");
        return 1;
    }

    usleep(500 * 1000); // 500 ms

    check(2, range, 29, std::move(numsFile));

    ifstream iNumsFile("~/Downloads/nums.txt");

    kill(pid, SIGTERM);
    sleep(2);
#endif

    string line;
    while (getline(iNumsFile, line)) {
        cout << line << endl;
#ifdef _WIN32
        Sleep(500);
#else
        usleep(500 * 1000);
#endif
    }

    iNumsFile.close();
    return 0;
}
