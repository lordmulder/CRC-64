/********************************************************************/
/* CRC-64 command-line tool, by LoRd_MuldeR <MuldeR2@GMX.de>        */
/* This work has been released under the CC0 1.0 Universal license! */
/********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#endif

static const int VERSION_MAJOR = 1;
static const int VERSION_MINOR = 0;
static const int VERSION_PATCH = 0;

#define OPT_HELPSC 0x01
#define OPT_BINARY 0x02
#define OPT_UPPERC 0x04
#define OPT_DECIML 0x08
#define OPT_NOPADD 0x10
#define OPT_IGNERR 0x20
#define OPT_SILENT 0x40

/* ======================================================================== */
/* Platform                                                                 */
/* ======================================================================== */

#ifdef _WIN32

#define PROGRAM_NAME L"crc64.exe"
#define CHAR wchar_t
#define MAIN wmain
#define STRCASECMP _wcsicmp
#define PRINTF wprintf
#define FPUTS fputws
#define FPRINTF fwprintf
#define FOPEN _wfopen
#define __T(X) L##X
#define T(X) __T(X)
#ifndef S_IFMT
#define S_IFMT _S_IFMT
#endif
#ifndef S_IFDIR
#define S_IFDIR _S_IFDIR
#endif
int _dowildcard = -1;

#else

#define PROGRAM_NAME "crc64"
#define CHAR char
#define MAIN main
#define STRCASECMP strcasecmp
#define PRINTF printf
#define FPUTS fputs
#define FPRINTF fprintf
#define FOPEN fopen
#define __T(X) X
#define T(X) __T(X)

#endif

/* ======================================================================== */
/* Signal handler                                                           */
/* ======================================================================== */

static volatile int g_stop_flag = 0;

static void sigint_handler(const int sig)
{
    if (sig == SIGINT)
    {
        g_stop_flag = 1;
    }
}

/* ======================================================================== */
/* CRC-64                                                                   */
/* ======================================================================== */

static const uint64_t CRC64_INITIALIZER = UINT64_C(~0);

static const uint64_t CRC64_TABLE[256] =
{
    0x0000000000000000ULL, 0x42f0e1eba9ea3693ULL, 0x85e1c3d753d46d26ULL, 0xc711223cfa3e5bb5ULL,
    0x493366450e42ecdfULL, 0x0bc387aea7a8da4cULL, 0xccd2a5925d9681f9ULL, 0x8e224479f47cb76aULL,
    0x9266cc8a1c85d9beULL, 0xd0962d61b56fef2dULL, 0x17870f5d4f51b498ULL, 0x5577eeb6e6bb820bULL,
    0xdb55aacf12c73561ULL, 0x99a54b24bb2d03f2ULL, 0x5eb4691841135847ULL, 0x1c4488f3e8f96ed4ULL,
    0x663d78ff90e185efULL, 0x24cd9914390bb37cULL, 0xe3dcbb28c335e8c9ULL, 0xa12c5ac36adfde5aULL,
    0x2f0e1eba9ea36930ULL, 0x6dfeff5137495fa3ULL, 0xaaefdd6dcd770416ULL, 0xe81f3c86649d3285ULL,
    0xf45bb4758c645c51ULL, 0xb6ab559e258e6ac2ULL, 0x71ba77a2dfb03177ULL, 0x334a9649765a07e4ULL,
    0xbd68d2308226b08eULL, 0xff9833db2bcc861dULL, 0x388911e7d1f2dda8ULL, 0x7a79f00c7818eb3bULL,
    0xcc7af1ff21c30bdeULL, 0x8e8a101488293d4dULL, 0x499b3228721766f8ULL, 0x0b6bd3c3dbfd506bULL,
    0x854997ba2f81e701ULL, 0xc7b97651866bd192ULL, 0x00a8546d7c558a27ULL, 0x4258b586d5bfbcb4ULL,
    0x5e1c3d753d46d260ULL, 0x1cecdc9e94ace4f3ULL, 0xdbfdfea26e92bf46ULL, 0x990d1f49c77889d5ULL,
    0x172f5b3033043ebfULL, 0x55dfbadb9aee082cULL, 0x92ce98e760d05399ULL, 0xd03e790cc93a650aULL,
    0xaa478900b1228e31ULL, 0xe8b768eb18c8b8a2ULL, 0x2fa64ad7e2f6e317ULL, 0x6d56ab3c4b1cd584ULL,
    0xe374ef45bf6062eeULL, 0xa1840eae168a547dULL, 0x66952c92ecb40fc8ULL, 0x2465cd79455e395bULL,
    0x3821458aada7578fULL, 0x7ad1a461044d611cULL, 0xbdc0865dfe733aa9ULL, 0xff3067b657990c3aULL,
    0x711223cfa3e5bb50ULL, 0x33e2c2240a0f8dc3ULL, 0xf4f3e018f031d676ULL, 0xb60301f359dbe0e5ULL,
    0xda050215ea6c212fULL, 0x98f5e3fe438617bcULL, 0x5fe4c1c2b9b84c09ULL, 0x1d14202910527a9aULL,
    0x93366450e42ecdf0ULL, 0xd1c685bb4dc4fb63ULL, 0x16d7a787b7faa0d6ULL, 0x5427466c1e109645ULL,
    0x4863ce9ff6e9f891ULL, 0x0a932f745f03ce02ULL, 0xcd820d48a53d95b7ULL, 0x8f72eca30cd7a324ULL,
    0x0150a8daf8ab144eULL, 0x43a04931514122ddULL, 0x84b16b0dab7f7968ULL, 0xc6418ae602954ffbULL,
    0xbc387aea7a8da4c0ULL, 0xfec89b01d3679253ULL, 0x39d9b93d2959c9e6ULL, 0x7b2958d680b3ff75ULL,
    0xf50b1caf74cf481fULL, 0xb7fbfd44dd257e8cULL, 0x70eadf78271b2539ULL, 0x321a3e938ef113aaULL,
    0x2e5eb66066087d7eULL, 0x6cae578bcfe24bedULL, 0xabbf75b735dc1058ULL, 0xe94f945c9c3626cbULL,
    0x676dd025684a91a1ULL, 0x259d31cec1a0a732ULL, 0xe28c13f23b9efc87ULL, 0xa07cf2199274ca14ULL,
    0x167ff3eacbaf2af1ULL, 0x548f120162451c62ULL, 0x939e303d987b47d7ULL, 0xd16ed1d631917144ULL,
    0x5f4c95afc5edc62eULL, 0x1dbc74446c07f0bdULL, 0xdaad56789639ab08ULL, 0x985db7933fd39d9bULL,
    0x84193f60d72af34fULL, 0xc6e9de8b7ec0c5dcULL, 0x01f8fcb784fe9e69ULL, 0x43081d5c2d14a8faULL,
    0xcd2a5925d9681f90ULL, 0x8fdab8ce70822903ULL, 0x48cb9af28abc72b6ULL, 0x0a3b7b1923564425ULL,
    0x70428b155b4eaf1eULL, 0x32b26afef2a4998dULL, 0xf5a348c2089ac238ULL, 0xb753a929a170f4abULL,
    0x3971ed50550c43c1ULL, 0x7b810cbbfce67552ULL, 0xbc902e8706d82ee7ULL, 0xfe60cf6caf321874ULL,
    0xe224479f47cb76a0ULL, 0xa0d4a674ee214033ULL, 0x67c58448141f1b86ULL, 0x253565a3bdf52d15ULL,
    0xab1721da49899a7fULL, 0xe9e7c031e063acecULL, 0x2ef6e20d1a5df759ULL, 0x6c0603e6b3b7c1caULL,
    0xf6fae5c07d3274cdULL, 0xb40a042bd4d8425eULL, 0x731b26172ee619ebULL, 0x31ebc7fc870c2f78ULL,
    0xbfc9838573709812ULL, 0xfd39626eda9aae81ULL, 0x3a28405220a4f534ULL, 0x78d8a1b9894ec3a7ULL,
    0x649c294a61b7ad73ULL, 0x266cc8a1c85d9be0ULL, 0xe17dea9d3263c055ULL, 0xa38d0b769b89f6c6ULL,
    0x2daf4f0f6ff541acULL, 0x6f5faee4c61f773fULL, 0xa84e8cd83c212c8aULL, 0xeabe6d3395cb1a19ULL,
    0x90c79d3fedd3f122ULL, 0xd2377cd44439c7b1ULL, 0x15265ee8be079c04ULL, 0x57d6bf0317edaa97ULL,
    0xd9f4fb7ae3911dfdULL, 0x9b041a914a7b2b6eULL, 0x5c1538adb04570dbULL, 0x1ee5d94619af4648ULL,
    0x02a151b5f156289cULL, 0x4051b05e58bc1e0fULL, 0x87409262a28245baULL, 0xc5b073890b687329ULL,
    0x4b9237f0ff14c443ULL, 0x0962d61b56fef2d0ULL, 0xce73f427acc0a965ULL, 0x8c8315cc052a9ff6ULL,
    0x3a80143f5cf17f13ULL, 0x7870f5d4f51b4980ULL, 0xbf61d7e80f251235ULL, 0xfd913603a6cf24a6ULL,
    0x73b3727a52b393ccULL, 0x31439391fb59a55fULL, 0xf652b1ad0167feeaULL, 0xb4a25046a88dc879ULL,
    0xa8e6d8b54074a6adULL, 0xea16395ee99e903eULL, 0x2d071b6213a0cb8bULL, 0x6ff7fa89ba4afd18ULL,
    0xe1d5bef04e364a72ULL, 0xa3255f1be7dc7ce1ULL, 0x64347d271de22754ULL, 0x26c49cccb40811c7ULL,
    0x5cbd6cc0cc10fafcULL, 0x1e4d8d2b65facc6fULL, 0xd95caf179fc497daULL, 0x9bac4efc362ea149ULL,
    0x158e0a85c2521623ULL, 0x577eeb6e6bb820b0ULL, 0x906fc95291867b05ULL, 0xd29f28b9386c4d96ULL,
    0xcedba04ad0952342ULL, 0x8c2b41a1797f15d1ULL, 0x4b3a639d83414e64ULL, 0x09ca82762aab78f7ULL,
    0x87e8c60fded7cf9dULL, 0xc51827e4773df90eULL, 0x020905d88d03a2bbULL, 0x40f9e43324e99428ULL,
    0x2cffe7d5975e55e2ULL, 0x6e0f063e3eb46371ULL, 0xa91e2402c48a38c4ULL, 0xebeec5e96d600e57ULL,
    0x65cc8190991cb93dULL, 0x273c607b30f68faeULL, 0xe02d4247cac8d41bULL, 0xa2dda3ac6322e288ULL,
    0xbe992b5f8bdb8c5cULL, 0xfc69cab42231bacfULL, 0x3b78e888d80fe17aULL, 0x7988096371e5d7e9ULL,
    0xf7aa4d1a85996083ULL, 0xb55aacf12c735610ULL, 0x724b8ecdd64d0da5ULL, 0x30bb6f267fa73b36ULL,
    0x4ac29f2a07bfd00dULL, 0x08327ec1ae55e69eULL, 0xcf235cfd546bbd2bULL, 0x8dd3bd16fd818bb8ULL,
    0x03f1f96f09fd3cd2ULL, 0x41011884a0170a41ULL, 0x86103ab85a2951f4ULL, 0xc4e0db53f3c36767ULL,
    0xd8a453a01b3a09b3ULL, 0x9a54b24bb2d03f20ULL, 0x5d45907748ee6495ULL, 0x1fb5719ce1045206ULL,
    0x919735e51578e56cULL, 0xd367d40ebc92d3ffULL, 0x1476f63246ac884aULL, 0x568617d9ef46bed9ULL,
    0xe085162ab69d5e3cULL, 0xa275f7c11f7768afULL, 0x6564d5fde549331aULL, 0x279434164ca30589ULL,
    0xa9b6706fb8dfb2e3ULL, 0xeb46918411358470ULL, 0x2c57b3b8eb0bdfc5ULL, 0x6ea7525342e1e956ULL,
    0x72e3daa0aa188782ULL, 0x30133b4b03f2b111ULL, 0xf7021977f9cceaa4ULL, 0xb5f2f89c5026dc37ULL,
    0x3bd0bce5a45a6b5dULL, 0x79205d0e0db05dceULL, 0xbe317f32f78e067bULL, 0xfcc19ed95e6430e8ULL,
    0x86b86ed5267cdbd3ULL, 0xc4488f3e8f96ed40ULL, 0x0359ad0275a8b6f5ULL, 0x41a94ce9dc428066ULL,
    0xcf8b0890283e370cULL, 0x8d7be97b81d4019fULL, 0x4a6acb477bea5a2aULL, 0x089a2aacd2006cb9ULL,
    0x14dea25f3af9026dULL, 0x562e43b4931334feULL, 0x913f6188692d6f4bULL, 0xd3cf8063c0c759d8ULL,
    0x5dedc41a34bbeeb2ULL, 0x1f1d25f19d51d821ULL, 0xd80c07cd676f8394ULL, 0x9afce626ce85b507ULL
};

static uint64_t crc64_update(uint64_t crc, const uint8_t *const buffer, const size_t count)
{
    const uint8_t *p = buffer;
    size_t i, t;
    for (i = 0U; i < count; ++i)
    {
        t = ((crc >> 56) ^ (*p++)) & 0xFF;
        crc = CRC64_TABLE[t] ^ (crc << 8);
    }
    return crc;
}

/* ======================================================================== */
/* Detect directory                                                         */
/* ======================================================================== */

#ifdef _MSC_VER
#pragma warning(disable: 4100)
#endif

static int is_directory(FILE *const file)
{
#ifndef _WIN32
    struct stat statbuf;
    if (!fstat(fileno(file), &statbuf))
    {
        if ((statbuf.st_mode & S_IFMT) == S_IFDIR)
        {
            return 1;
        }
    }
#endif
    return 0;
}

/* ======================================================================== */
/* Process File                                                             */
/* ======================================================================== */

const CHAR *const STR_STDIN = T("/dev/stdin");

#define BUFF_SIZE 4096U

static int process(const CHAR *const file_name, const int options)
{
    int retval = EXIT_FAILURE;
    FILE *input = NULL;
    uint64_t crc = CRC64_INITIALIZER, total_size = 0U;
    uint8_t buffer[BUFF_SIZE];

    input = file_name ? FOPEN(file_name, T("rb")) : stdin;
    if (!input)
    {
        if (!(options & OPT_SILENT))
        {
            const int error_code = errno;
            FPRINTF(stderr, T("Error: Failed to open input \"%s\" file for reading! (error #%d)\n"), file_name ? file_name : STR_STDIN, error_code);
        }
        return EXIT_FAILURE;
    }

    if (is_directory(input))
    {
        if (!(options & OPT_SILENT))
        {
            FPRINTF(stderr, T("Error: Input file \"%s\" is a directory!\n"), file_name ? file_name : STR_STDIN);
        }
        goto clean_up;
    }

    while ((!feof(input)) && (!g_stop_flag))
    {
        const size_t count = fread(buffer, sizeof(uint8_t), BUFF_SIZE, input);
        if (count > 0U)
        {
            total_size += count;
            crc = crc64_update(crc, buffer, count);
        }
        if (ferror(input))
        {
            if (!(options & OPT_IGNERR))
            {
                if (!(options & OPT_SILENT))
                {
                    FPUTS(T("I/O Error: Failed to read input data!\n"), stderr);
                }
                goto clean_up;
            }
            else
            {
                break; /*ignore the read error*/
            }
        }
    }

    if (!(options & OPT_BINARY))
    {
        if (!(options & OPT_DECIML))
        {
            if (!(options & OPT_UPPERC))
            {
                if (!(options & OPT_NOPADD))
                {
                    PRINTF(T("%016") T(PRIx64) T(" %016") T(PRIx64) T(" %s\n"), crc, total_size, file_name ? file_name : STR_STDIN);
                }
                else
                {
                    PRINTF(T("%") T(PRIx64) T(" %") T(PRIx64) T(" %s\n"), crc, total_size, file_name ? file_name : STR_STDIN);
                }
            }
            else
            {
                if (!(options & OPT_NOPADD))
                {
                    PRINTF(T("%016") T(PRIX64) T(" %016") T(PRIX64) T(" %s\n"), crc, total_size, file_name ? file_name : STR_STDIN);
                }
                else
                {
                    PRINTF(T("%") T(PRIX64) T(" %") T(PRIX64) T(" %s\n"), crc, total_size, file_name ? file_name : STR_STDIN);
                }
            }
        }
        else
        {
            if (!(options & OPT_NOPADD))
            {
                PRINTF(T("%020") T(PRIu64) T(" %020") T(PRIu64) T(" %s\n"), crc, total_size, file_name ? file_name : STR_STDIN);
            }
            else
            {
                PRINTF(T("%") T(PRIu64) T(" %") T(PRIu64) T(" %s\n"), crc, total_size, file_name ? file_name : STR_STDIN);
            }
        }
    }
    else
    {
        fwrite(&crc, sizeof(uint64_t), 1U, stdout);
    }

    retval = EXIT_SUCCESS;

clean_up:

    if (input && (input != stdin))
    {
        fclose(input);
    }

    return retval;
}

/* ======================================================================== */
/* MAIN                                                                     */
/* ======================================================================== */

int MAIN(int argc, CHAR *argv[])
{
    int arg_off = 1, options = 0, exit_code = EXIT_SUCCESS;

#ifdef _WIN32
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    _setmode(_fileno(stdin),  _O_BINARY);
    _setmode(_fileno(stderr), _O_U8TEXT);
    signal(SIGINT, sigint_handler);
#else
    struct sigaction act;
    act.sa_handler = sigint_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
#endif

    while ((arg_off < argc) && (argv[arg_off][0] == '-') && (argv[arg_off][1] == '-'))
    {
        const CHAR *const arg_val = argv[arg_off++] + 2U;
        if (arg_val[0] != '\0')
        {
            if ((!STRCASECMP(arg_val, T("help"))) || (!STRCASECMP(arg_val, T("version"))))
            {
                options |= OPT_HELPSC;
            }
            else if (!STRCASECMP(arg_val, T("binary")))
            {
                options |= OPT_BINARY;
            }
            else if (!STRCASECMP(arg_val, T("upper-case")))
            {
                options |= OPT_UPPERC;
            }
            else if (!STRCASECMP(arg_val, T("decimal")))
            {
                options |= OPT_DECIML;
            }
            else if (!STRCASECMP(arg_val, T("no-padding")))
            {
                options |= OPT_NOPADD;
            }
            else if (!STRCASECMP(arg_val, T("ignore-errors")))
            {
                options |= OPT_IGNERR;
            }
            else if (!STRCASECMP(arg_val, T("silent")))
            {
                options |= OPT_SILENT;
            }
            else
            {
                FPRINTF(stderr, T("Option \"--%s\" is not recognized!\n"), arg_val);
                return EXIT_FAILURE;
            }
        }
        else
        {
            break; /*stop option processing*/
        }
    }

    if ((options & OPT_BINARY) && (options & OPT_DECIML))
    {
        FPUTS(T("Error: Binary output and decimal output are mutually exclusive!\n"), stderr);
        return EXIT_FAILURE;
    }

    if (options & OPT_HELPSC)
    {
        FPRINTF(stderr, T("CRC64 %d.%d.%d [%s]\n\n"), VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, T(__DATE__));
        FPUTS(T("Synopsis:\n"), stderr);
        FPUTS(T("   ") PROGRAM_NAME T(" [OPTIONS] [<file_1> [<file_2> ... <file_n>]]\n\n"), stderr);
        FPUTS(T("Options:\n"), stderr);
        FPUTS(T("   --help --version  Print help screen / show version information\n"), stderr);
        FPUTS(T("   --binary          Output the digest in binary format (default is hex-format)\n"), stderr);
        FPUTS(T("   --decimal         Output the digest in decimal string format\n"), stderr);
        FPUTS(T("   --upper-case      Print hex-string in upper-case letters (default is lower-case)\n"), stderr);
        FPUTS(T("   --no-padding      Print the digest *without* any leading zeros\n"), stderr);
        FPUTS(T("   --silent          Suppress error messages\n"), stderr);
        FPUTS(T("   --ignore-errors   Ignore I/O errors and proceeed with the next file\n\n"), stderr);
        return EXIT_SUCCESS;
    }

#ifdef _WIN32
    _setmode(_fileno(stdout), (options & OPT_BINARY) ? _O_BINARY : _O_U8TEXT);
#endif

    if (arg_off < argc)
    {
        while ((arg_off < argc) && (!g_stop_flag))
        {
            if (process(argv[arg_off++], options) != EXIT_SUCCESS)
            {
                if (!(options & OPT_IGNERR))
                {
                    exit_code = EXIT_FAILURE;
                    break;
                }
            }
        }
    }
    else
    {
        if (process(NULL, options) != EXIT_SUCCESS)
        {
            if (!(options & OPT_IGNERR))
            {
                exit_code = EXIT_FAILURE;
            }
        }
    }

    if (g_stop_flag && (!(options & OPT_SILENT)))
    {
        FPUTS(T("Error: Process was interrupted!\n"), stderr);
        if (!(options & OPT_IGNERR))
        {
            exit_code = 128 + SIGINT;
        }
    }

    return exit_code;
}
