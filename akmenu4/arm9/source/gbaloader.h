#ifndef __GBALOADER_H__
#define __GBALOADER_H__

class CGbaLoader
{
  private:
    std::string iFileName;
    u32 iSize;
  private:
    CGbaLoader();
    void LoadBorder(void);
    void BootGBA(void);
    bool LoadPSRAM(void);
    bool LoadNor(void);
    bool LoadInternal(bool nor);
    void InitNor(void);
    void InitPSRAM(void);
    bool StoreOldSave(std::string& aFileName);
  private: 
	bool LoadPsramEwin(void);
    bool LoadInternalEwin(void);
  public:
    CGbaLoader(const std::string& aFileName);
    bool Load(void);
	void LaunchGBA(void);
};

#endif
