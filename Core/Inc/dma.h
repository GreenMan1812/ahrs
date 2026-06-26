#ifndef DMA_H_
#define DMA_H_
#define Dma_Disable(stream) do {\
stream->CR &= ~DMA_SxCR_EN; \
while((stream->CR & DMA_SxCR_EN));\
} while(0)

#define Dma_Start(stream, data, buffSize)do{\
	Dma_Disable(stream);\
    stream->M0AR = (uint32_t)data;\
    stream->NDTR = buffSize;\
    stream->CR |= DMA_SxCR_EN;\
}while(0)

#define DMA_CHSEL(channel) (((uint32_t)(channel)) << DMA_SxCR_CHSEL_Pos)
#endif
