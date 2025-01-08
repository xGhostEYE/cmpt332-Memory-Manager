/*# Dwight Makaroff   */
/*# student number    */
/*# nsid              */
/*# November 7, 2008  */

/* modified slightly October 2018 */

/* No part of this program may be used for any profit or example for any
   purpose other than for help in A2 for CMPT 332 at the University
   of Saskatchewan in the fall term of the current academic year
   without the expressed
   written consent of the author.
   

   *********** reader-writer.c ***********
*/

/* CMPT 332
* Assignment 4
* Group 38
* Tanner Laroque, YJE740, 11122821
* Melvin Lugo, 11279686, mel196
*/

#include <stdio.h>


#include <monitor.h>


/* INITIALIZATION CODE */
#define OKtoWrite 1
#define OKtoRead  0
#define numConds  2


static int numReaders = 0;
static int busyWriting =0;

/* MONITOR PROCEDURES */
void Initialize(void)
{
      RttMonInit(numConds);
}

void StartRead()
{
  RttMonEnter();

  /* if there is a writer */
  if (busyWriting)
    {
      RttMonWait(OKtoRead);
    }
  /* once we are signaled, we can continue.
   * There will be at least one reader, so we can be assured
   * that no writers will get to go. No other readers will get
   * to go if we do not signal the rest of the readers, so each
   * reader that wakes up does a signal on OKtoRead. This will be OK
   * since the last signal will have no effect on a late-coming
   * reader.
   */
  numReaders++;
  printf("starting value of numReaders = %d\n", numReaders);
  
  RttMonSignal(OKtoRead);
  
  RttMonLeave();

}

void StopRead()
{
  RttMonEnter();
  numReaders--;
  printf("stopping value of numReaders = %d\n", numReaders);
  /* Only signal the writer when all readers have gone */
  if (numReaders == 0) RttMonSignal(OKtoWrite);
  RttMonLeave();
  
  
}

void StartWrite()
{
  RttMonEnter(); 

  /* if there are any readers or a writer */
  if ((numReaders !=0) || busyWriting)
    RttMonWait(OKtoWrite);
  busyWriting = 1;
  RttMonLeave();
}

void StopWrite()
{
  RttMonEnter();

  busyWriting = 0;

  /* wake up any waiting readers */
  /* this gives priority to readers */
  /* could be a signal to the next writer, if there is one, but
   * then we'd need a mechanism to eventually signal a reader.
   * We couldn't signal a writer and then immediately signal a reader.
  */
  RttMonSignal(OKtoRead);

  RttMonLeave();

}

