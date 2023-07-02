#include <core/core.h>
#include <efi.h>
#include <efiapi.h>
#include <efibind.h>
#include <eficon.h>
#include <efidef.h>
#include <efierr.h>
#include <efiprot.h>
#include <platform/platform.h>

EFI_BOOT_SERVICES *BS;
EFI_SYSTEM_TABLE *ST;
EFI_HANDLE Image;

#define OVEFI_MEMORY EfiLoaderData

size_t strlena(char *str) {
  size_t i = 0;
  while (str[++i] != 0)
    ;
  return i;
}

void Print(CHAR16 *str) { ST->ConOut->OutputString(ST->ConOut, str); }

CHAR16 *str_to_wstr(char *str) {
  CHAR16 *wstr;
  BS->AllocatePool(OVEFI_MEMORY, strlena(str) + 1, (void **)&wstr);
  for (size_t i = 0; i < strlena(str); i++) {
    wstr[i] = str[i];
  };
  wstr[strlena(str)] = 0x00;
  return wstr;
}

void func_log(char *section, char *format, ...) {
  Print(L"[");
  Print(str_to_wstr(section));
  Print(L"] ");
  Print(str_to_wstr(format));
  Print(L"\r\n");
}

EFI_FILE_HANDLE GetVolume() {
  EFI_LOADED_IMAGE *loaded_image = NULL;
  EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
  EFI_FILE_IO_INTERFACE *IOVolume;
  EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
  EFI_FILE_HANDLE Volume;

  // get the loaded image protocol interface for our image
  BS->HandleProtocol(Image, &lipGuid, (void **)&loaded_image);
  // get the volume handle
  BS->HandleProtocol(loaded_image->DeviceHandle, &fsGuid, (void *)&IOVolume);
  IOVolume->OpenVolume(IOVolume, &Volume);
  return Volume;
}

EFI_FILE_HANDLE FileHandle;

bool func_fopen() {
  EFI_FILE_HANDLE Volume = GetVolume();
  EFI_STATUS Status =
      Volume->Open(Volume, &FileHandle, L"VIDEO.BIN", EFI_FILE_MODE_READ,
                   EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
  if (EFI_ERROR(Status))
    return false;
  return true;
}

bool func_fclose() {
  FileHandle->Close(FileHandle);
  return true;
}

uint8_t *func_fread(size_t count) {
  uint8_t *buffer;
  if (EFI_ERROR(BS->AllocatePool(OVEFI_MEMORY, count, (void **)&buffer)))
    return NULL;
  FileHandle->Read(FileHandle, &count, buffer);
  return buffer;
}

int func_strncmp(const char *str1, const char *str2, size_t n) {
  while (n && *str1 && (*str1 == *str2)) {
    ++str1;
    ++str2;
    --n;
  }
  if (n == 0)
    return 0;
  return *(unsigned char *)str1 - *(unsigned char *)str2;
}

void *func_malloc(size_t size) {
  void *ret;
  if (EFI_ERROR(BS->AllocatePool(OVEFI_MEMORY, size, (void **)&ret)))
    return NULL;
  return ret;
}

void *func_memset(void *dest, int value, size_t size) {
  int i = 0;
  unsigned char *p = dest;
  while (size > 0) {
    *p = value;
    p++;
    size--;
  }
  return dest;
}

void func_free(void *ptr) { BS->FreePool(ptr); }

EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

int l_width;
int l_height;

bool func_display_open(int width, int height) {
  l_width = width;
  l_height = height;

  // locate the GOP
  EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  EFI_STATUS Status = BS->LocateProtocol(&gopGuid, NULL, (void **)&gop);
  if (EFI_ERROR(Status)) {
    Print(L"[EFI] Failed to locate GOP\r\n");
    return false;
  }

  // get the current mode
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
  UINTN SizeOfInfo;
  Status = gop->QueryMode(gop, gop->Mode == NULL ? 0 : gop->Mode->Mode,
                          &SizeOfInfo, &info);
  if (Status == EFI_NOT_STARTED) {
    Status = gop->SetMode(gop, 0);
  }
  if (EFI_ERROR(Status)) {
    Print(L"[EFI] Unable to get native mode\r\n");
    return false;
  }

  // we're gonna be using the native mode for rendering
  // so we're done i think!
  return true;
}

bool func_display_close() { return true; }

uint32_t funny_pixel_shit(uint8_t pixel) {
  return 0xFF000000 + (pixel << 16) + (pixel << 8) + pixel;
}

void func_display_frame(uint8_t *framebuffer) {
  // painful but what can ya do
  for (int i = 0; i < l_width * l_height; i++) {
    int x = i % l_width;
    int y = (i - x) / l_width;
    int new_i = y * gop->Mode->Info->PixelsPerScanLine * 4 + x * 4;
    *((uint32_t *)(gop->Mode->FrameBufferBase + new_i)) =
        funny_pixel_shit(framebuffer[i]);
  }
}

void msleep(unsigned long msecs) {
  if (msecs > 1000)
    return; // dont stall for more than 1s
  BS->Stall(msecs * 1000);
}

long long get_ms_time() {
  EFI_TIME Time;
  EFI_TIME_CAPABILITIES TimeCapabilities;
  ST->RuntimeServices->GetTime(&Time, &TimeCapabilities);
  return Time.Second * 1000 + (Time.Nanosecond / 1000 / 1000);
}

struct omavideo_platform_funcs funcs = {
    .log = *func_log,
    .msleep = *msleep,
    .get_ms_time = *get_ms_time,

    .malloc = *func_malloc,
    .free = *func_free,
    .memset = *func_memset,
    .strncmp = *func_strncmp,

    .fopen = *func_fopen,
    .fread = *func_fread,
    .fclose = *func_fclose,

    .display_open = *func_display_open,
    .display_frame = *func_display_frame,
    .display_close = *func_display_close,
};

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *sysTable) {
  ST = sysTable;
  BS = ST->BootServices;
  Image = image;
  BS->SetWatchdogTimer(0, 0, 0, NULL);
  Print(L"starting omavideo...\r\n");

  omavideo_init(&funcs);

  return EFI_SUCCESS;
}
