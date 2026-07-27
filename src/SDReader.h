#ifndef SDREADER_H
#define SDREADER_H

#include <vector>
#include <algorithm>
#include <SD.h>

/**
    @brief  Тип файла
*/
enum class AudioFileEntryType {
    Directory,  // Каталог
    MP3,        // MP3-файл
    WAV,        // WAV
    Unknown     // Неизвестный (все остальные файлы, кроме перечисленных выше)
};

struct AudioFileEntry {
    String name;              // Имя файла
    AudioFileEntryType type;  // Тип аудио файла
};

// Тестовые данные
std::vector<AudioFileEntry> files = {
    { "B file MP3", AudioFileEntryType::MP3 },
    { "Русский файл MP3", AudioFileEntryType::MP3 },
    { "Ещё русский каталог", AudioFileEntryType::Directory },
    { "A dir", AudioFileEntryType::Directory },
    { "C dir", AudioFileEntryType::Directory },
    { "WAV", AudioFileEntryType::WAV },
    { "Русский WAV", AudioFileEntryType::WAV },
};
/**
    @brief Класс для чтения SD-карты

    - установка текущего каталога;
    - чтение текущего каталога;
    - фильтрация файлов;
    - сортировка файлов и каталогов;
*/
class SDReader {
  public:
    void sort(std::vector<AudioFileEntry>& files) {
        std::sort(files.begin(), files.end(), [](const AudioFileEntry& a, const AudioFileEntry& b) {
            if (a.type != b.type) {
                return a.type < b.type;  // Первый признак (возрастание)
            }
            return a.name.compareTo(b.name) < 0;
        });
    }  // sort


    void read(void) {
    }

    void setCurrentDir(String dir) {
        currentDir = dir;
        read();
    }
  protected:
    String currentDir;
    std::vector<AudioFileEntry> entries;
    /**
        @brief Возвращает тип найденного файла.
        @param entry Указатель на файл.
        @returns Тип файла.
    */
    AudioFileEntryType getEntryType(fs::File* entry) {
        if (entry->isDirectory())
            return AudioFileEntryType::Directory;
        if (entry->isFile()) {
            String entryName = String(entry->name());
            entryName.toLowerCase();
            if (entryName.endsWith(".mp3"))
                return AudioFileEntryType::MP3;
            if (entryName.endsWith(".wav"))
                return AudioFileEntryType::WAV;
        }
        return AudioFileEntryType::Unknown;
    }  // getEntryType
};

#endif