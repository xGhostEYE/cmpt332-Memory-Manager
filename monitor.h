/* CMPT 332
* Assignment 4
* Group 38
* Tanner Laroque, YJE740, 11122821
* Melvin Lugo, 11279686, mel196
*/

#ifndef MONITORS
#define MONITORS
void RttMonInit();
void RttMonEnter();
void RttMonLeave();
void RttMonWait(int CV);
void RttMonSignal(int CV);
void RttMonServer();
#endif
