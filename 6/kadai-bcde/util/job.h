#ifndef JOB_H_INCLUDED
#define JOB_H_INCLUDED

#define PROCESS_END -1

typedef struct backProcessList{
    int *pids; //pidを入れる。終了したことが確認されたらPROCESS_ENDが入る
    int processN;
    struct backProcessList *next;
    struct backProcessList *previous;
}BackProcessList;

//バックグラウンドで実行中のジョブの管理などを行う
void addBackground(int [], int, BackProcessList**);
void checkBackProcess(BackProcessList **);
void freeBackProcess(BackProcessList *);
void addProcessList(BackProcessList **, BackProcessList **);
void printBackProcessList(BackProcessList *);
#endif