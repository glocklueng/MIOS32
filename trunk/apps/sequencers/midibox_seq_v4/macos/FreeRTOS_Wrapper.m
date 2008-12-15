//
//  FreeRTOS_Wrapper.m
//
//  Created by Thorsten Klose on 05.12.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "FreeRTOS_Wrapper.h"


@implementation FreeRTOS_Wrapper

// local variables to bridge objects to C functions
static NSObject *_self;


//////////////////////////////////////////////////////////////////////////////
// init local variables
//////////////////////////////////////////////////////////////////////////////
- (void) awakeFromNib
{
	_self = self;
	
	// install background task
	NSTimer *timer1 = [NSTimer timerWithTimeInterval:0.01 target:self selector:@selector(backgroundTask:) userInfo:nil repeats:YES];
	[[NSRunLoop currentRunLoop] addTimer: timer1 forMode: NSRunLoopCommonModes];
}


//////////////////////////////////////////////////////////////////////////////
// the background task
// (in real HW called whenever nothing else is to do...
//////////////////////////////////////////////////////////////////////////////
- (void)backgroundTask:(NSTimer *)aTimer
{
	// -> forward to application
	APP_Background();
}



//////////////////////////////////////////////////////////////////////////////
// wrapper for FreeRTOS malloc functions
//////////////////////////////////////////////////////////////////////////////
void *pvPortMalloc(size_t xWantedSize)
{
	return malloc(xWantedSize);
}

void vPortFree( void *pv )
{
	free(pv);
}



@end
