/*
 * =============================================================================
 * Tên tệp       : uaf_vuln.c
 * Kịch bản       : Use After Free (CWE-416) - #5 CWE Top 25 (2023)
 * Mô tả         : Phiên bản CÓ LỖ HỔNG - sử dụng con trỏ sau khi free()
 *
 * So với bản gốc, thay đổi:
 *   - Hàm xu_ly_user() dùng malloc() cấp phát, free() giải phóng
 *   - Sau khi free(), con trỏ vẫn được sử dụng -> UAF
 *
 * Lỗ hổng:
 *   - CWE-416: Use After Free
 *   - CWE-415: Double Free
 *   - CVE liên quan: CVE-2019-0708 (BlueKeep - Windows RDP), CVE-2022-22965 (Spring4Shell)
 *   - Rủi ro: attacker cấp phát lại vùng nhớ đó với dữ liệu kiểm soát được
 *     -> thực thi mã tùy ý (RCE)
 *
 * Dùng trong demo: SCGL phải phát hiện commit này là BUGGY.
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KICH_THUOC_MANG    10

typedef struct {
    int  ma_nguoi_dung;
    char ten[64];
    int  tuoi;
} NguoiDung;

NguoiDung* tao_nguoi_dung(int ma, const char *ten, int tuoi) {
    NguoiDung *nd = (NguoiDung *)malloc(sizeof(NguoiDung));
    if (nd == NULL) return NULL;
    nd->ma_nguoi_dung = ma;
    strncpy(nd->ten, ten, sizeof(nd->ten) - 1);
    nd->ten[sizeof(nd->ten) - 1] = '\0';
    nd->tuoi = tuoi;
    return nd;
}

void in_nguoi_dung(const NguoiDung *nd) {
    if (nd == NULL) return;
    printf("[User] ma=%d, ten=%s, tuoi=%d\n",
           nd->ma_nguoi_dung, nd->ten, nd->tuoi);
}

/* HÀM NÀY CHỨA LỖ HỔNG USE AFTER FREE */
void xu_ly_sai(NguoiDung *nd) {
    if (nd == NULL) return;
    in_nguoi_dung(nd);
    free(nd);  /* Giải phóng bộ nhớ */
    /* LỖI: Sử dụng con trỏ sau khi free -> UAF! */
    printf("[CANH BAO] UAF: truy cap nd->ten SAU free(): %s\n", nd->ten);
    /* LỖI nghiêm trọng hơn: ghi vào vùng nhớ đã free */
    nd->ma_nguoi_dung = -1;  /* Ghi vào vùng nhớ đã giải phóng */
}

int main(int argc, char **argv) {
    NguoiDung *nd = tao_nguoi_dung(1, "Nguyen Van A", 25);
    if (nd == NULL) return 1;

    xu_ly_sai(nd);

    /* LỖI: nd đã bị free nhưng vẫn dùng */
    in_nguoi_dung(nd);  /* UAF: truy cap vung nho da free */

    return 0;
}
