#include "image.h"

// BMP  8/24 bits
my_image *loadBMP(boost::filesystem::path & aPath, int nbytes, bool inverse)
{
	if ( nbytes != 0 && nbytes != 1 && nbytes != 3 ) return NULL;

	FILE *fp;
	fopen_s(&fp, aPath.file_string().data(), "rb");
	if (!fp) return NULL;

	BITMAPINFO *pbmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 256*4);
	if (!pbmi) { fclose(fp); return NULL; }

	fseek(fp, sizeof(BITMAPFILEHEADER), SEEK_SET);
	fread(&pbmi->bmiHeader, 1, sizeof(BITMAPINFOHEADER), fp);

	// кол-во байтов на пиксель в открываемом изображении
	int samples = pbmi->bmiHeader.biBitCount / 8;
	if (nbytes == 0) nbytes = samples;

	// читаем только 8 и 24-битные изображения
	if (samples != 1 && samples != 3) {
		fclose(fp);
		if (pbmi) free(pbmi);
		return NULL;
	}

	if (samples == 1)
		fread(pbmi->bmiColors, 256 * 4, 1, fp);

	int width  = pbmi->bmiHeader.biWidth;
	int height = pbmi->bmiHeader.biHeight;

	// создаём новое изображение
	my_image *image = image_create(width, height, nbytes);

	if (!image) {
		fclose(fp);
		if (pbmi) free(pbmi);
		return NULL;
	}

	// длина строки в открываемом и новом изображениях
	int buflen		= ((width * samples + 3)/4)*4;
	int step		= image->step;
	//int rowlen		= width * nbytes;

	// флаг: прочитать файл за раз или читать построчно
	bool read_once = true;
	// TODO: можно добавить проверку наличия свободной памяти,
	// а пока так: смогли выделить сразу - хорошо, не смогли - читаем построчно

	// выделяем память под буфер
	UCHAR *buf;

	if (read_once) {
		buf = (UCHAR*)malloc(buflen * height);
		if (!buf) { // не получилось выделить память
			read_once = false;
		} else { // читаем весь файл
			fread(buf, buflen, height, fp);
			fclose(fp);
		}
	}

	// будем читать построчно
	if (!read_once)
		buf = (UCHAR*)malloc(buflen);

	// не получилось выделить память, выходим
	if ( !buf ) {
		fclose(fp);
		if (pbmi) free(pbmi);
		image_release(image);
		return NULL;
	}

	int i, j, gray, num;

	UCHAR *src = buf;
	UCHAR *dst = image->data;

	if (!inverse) {
		dst = image->data + (height - 1) * step;
		step = -step;
	}

	if (nbytes == 3) {

		if (read_once) {

			if (samples == 1) {
				for (j = 0; j < height; j++) {
					num = 0;
					for (i = 0; i < width; i++) {
						RGBQUAD *sp		= pbmi->bmiColors + src[i];
						RGBTRIPLE *t	= (RGBTRIPLE*)(dst + num);
						t->rgbtRed		= sp->rgbBlue;
						t->rgbtGreen	= sp->rgbGreen;
						t->rgbtBlue		= sp->rgbRed;
						num += 3;
					}
					dst += step;
					src += buflen;
				}
			} else if (samples == 3) {
				num = 3 * width;
				for (j = 0; j < height; j++) {
					for (i = 0; i < num; i += 3) {
						dst[i + 0] = src[i + 2];
						dst[i + 1] = src[i + 1];
						dst[i + 2] = src[i + 0];
					}
					//memcpy(dst, buf, abs(rowlen));
					dst += step;
					src += buflen;
				}
			}

		} else {

			if (samples == 1) {
				for (j = 0; j < height; j++) {
					num = 0;
					fread(buf, buflen, 1, fp);
					for (i = 0; i < width; i++) {
						RGBQUAD *sp		= pbmi->bmiColors + buf[i];
						RGBTRIPLE *t	= (RGBTRIPLE*)(dst + num);
						t->rgbtRed		= sp->rgbBlue;
						t->rgbtGreen	= sp->rgbGreen;
						t->rgbtBlue		= sp->rgbRed;
						num += 3;
					}
					dst += step;
				}
			} else if (samples == 3) {
				num = 3 * width;
				for (j = 0; j < height; j++) {
					fread(buf, buflen, 1, fp);
					for (i = 0; i < num; i += 3) {
						dst[i + 0] = buf[i + 2];
						dst[i + 1] = buf[i + 1];
						dst[i + 2] = buf[i + 0];
					}
					//memcpy(dst, buf, abs(rowlen));
					dst += step;
				}
			}

		}

	}

	if (nbytes == 1) {

		if (read_once) {

			if (samples == 1) {
				for (j = 0; j < height; j++) {
					for (i = 0; i < width; i++) {
						RGBQUAD *quad = pbmi->bmiColors + src[i];
						gray = 218 * quad->rgbRed + 732 * quad->rgbGreen + 74 * quad->rgbBlue;
						dst[i] = gray >> 10;
					}
					dst += step;
					src += buflen;
				}
			} else if (samples == 3) {
				for (j = 0; j < height; j++) {
					num = 0;
					for (i = 0; i < width; i++) {
						RGBTRIPLE *triple = (RGBTRIPLE*)(src + num);
						gray = 218 * triple->rgbtRed + 732 * triple->rgbtGreen + 74 * triple->rgbtBlue;
						dst[i] = gray >> 10;
						num += 3;
					}
					dst += step;
					src += buflen;
				}
			}

		} else {

			if (samples == 1) {
				for (j = 0; j < height; j++) {
					fread(buf, buflen, 1, fp);
					for (i = 0; i < width; i++) {
						RGBQUAD *quad = pbmi->bmiColors + buf[i];
						gray = 218 * quad->rgbRed + 732 * quad->rgbGreen + 74 * quad->rgbBlue;
						dst[i] = gray >> 10;
					}
					dst += step;
				}
			} else if (samples == 3) {
				for (j = 0; j < height; j++) {
					num = 0;
					fread(buf, buflen, 1, fp);
					for (i = 0; i < width; i++) {
						RGBTRIPLE *triple = (RGBTRIPLE*)(buf + num);
						gray = 218 * triple->rgbtRed + 732 * triple->rgbtGreen + 74 * triple->rgbtBlue;
						dst[i] = gray >> 10;
						num += 3;
					}
					dst += step;
				}
			}

		}

	}

	if (!read_once)	fclose(fp);

	if (buf)	free(buf);
	if (pbmi)	free(pbmi);

	return image;
}