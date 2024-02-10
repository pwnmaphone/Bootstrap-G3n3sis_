#ifndef bootstrap_h
#define bootstrap_h

#define BOOTSTRAP_VERSION   (5)

#import <Foundation/Foundation.h>

static BOOL runSBINJECTOR;

void rebuildSignature(NSString *directoryPath);

int bootstrap();

int unbootstrap();

bool isBootstrapInstalled();

bool isSystemBootstrapped();

bool checkBootstrapVersion();

bool SBInjectionEnvironmentCheck();
#endif /* bootstrap_h */
