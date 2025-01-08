/* reader-writer.h
 * Dwight Makaroff October 27, 2011
 * necessary because I split up the 2 source files
 * for the reader-writer problem
 */
/* CMPT 332
* Assignment 4
* Group 38
* Tanner Laroque, YJE740, 11122821
* Melvin Lugo, 11279686, mel196
*/
#ifndef RWM_H
#define RWM_H
void Initialize(void);
void StartRead(void);
void StopRead(void);
void StartWrite(void);
void StopWrite(void);
#endif
