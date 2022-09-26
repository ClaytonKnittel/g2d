
#import <MetalKit/MetalKit.h>

@interface G2DLibrary : NSObject

@property (readonly, nonnull) id<MTLDevice> device;

@property (readonly, nonnull) id<MTLLibrary> library;

/*
 * Initialized the library using the global compiled metal shader library.
 */
- (nonnull instancetype)init;

@end
