#ifndef GFX_FONT
#define GFX_FONT

#include "SFont.h"

#include <string>

class gfxFont
{
	public:
		gfxFont();
		~gfxFont();

		bool init(const std::string& filename);
		void draw(int x, int y, const std::string& s);
		void drawf(int x, int y, const char *s, ...);

		void drawCentered(int x, int y, const char *text);
		void drawChopCentered(int x, int y, int width, const char *text);
		void drawRightJustified(int x, int y, const char *s, ...);
		void drawChopRight(int x, int y, int width, const char *s);
		void drawChopLeft(int x, int y, int width, const char *s);

		void setalpha(Uint8 alpha);

    int getHeight() {
        return SFont_TextHeight(m_font);
    };
    int getWidth(const char *text) {
        return SFont_TextWidth(m_font, text);
    };

	private:
		SFont_Font *m_font;
};

#endif // GFX_FONT
