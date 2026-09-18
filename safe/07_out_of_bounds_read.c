/*
 * =============================================================================
 * Tên tệp       : oob_read_safe.c
 * Kịch bản       : Out-of-bounds Read (CWE-125) - PHIÊN BẢN ĐÃ SỬA
 * Mô tả         : Phiên bản an toàn - kiểm tra biên nghiêm ngặt trước khi đọc
 *
 * So với bản gốc, thay đổi:
 *   - Thêm kiểm tra biên: 0 <= vi_tri < kich_thuoc
 *   - Trả về mã lỗi khi vi_tri ngoài phạm vi
 *   - Không cho phép đọc ngoài vùng nhớ đã cấp phát
 *
 * Đặc điểm:
 *   - CWE-125 fix: bounds checking trước mọi thao tác đọc
 *   - CWE-119 fix: phòng thủ chống truy cập bộ nhớ ngoài ý muốn
 *   - CWE-200 fix: ngăn chặn information disclosure qua OOB read
 *
 * Dùng trong demo: SCGL phải cho phép commit (CLEAN).
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KICH_THUOC_MANG    10

int khoi_tao_mang(int *mang, int kich_thuoc) {
    if (mang == NULL || kich_thuoc <= 0) return -1;
    for (int i = 0; i < kich_thuoc; i++) {
        mang[i] = i * 10;
    }
    return 0;
}

/* HÀM AN TOÀN - kiểm tra biên trước khi đọc */
int doc_phan_tu(const int *mang, int kich_thuoc, int vi_tri, int *gia_tri) {
    if (mang == NULL || gia_tri == NULL) {
        printf("[LOI] Con tro NULL\n");
        return -1;
    }
    /* Kiểm tra biên */
    if (vi_tri < 0 || vi_tri >= kich_thuoc) {
        printf("[TU CHOI] Vi tri %d ngoai pham vi [0, %d)\n",
               vi_tri, kich_thuoc);
        return -1;
    }
    *gia_tri = mang[vi_tri];
    return 0;
}

int main(int argc, char **argv) {
    int mang[KICH_THUOC_MANG];
    khoi_tao_mang(mang, KICH_THUOC_MANG);

    int gia_tri = 0;
    if (doc_phan_tu(mang, KICH_THUOC_MANG, 3, &gia_tri) == 0) {
        printf("[Hop le] mang[3] = %d\n", gia_tri);
    }

    /* Đọc ngoài phạm vi - bị từ chối, không lộ dữ liệu nhạy cảm */
    doc_phan_tu(mang, KICH_THUOC_MANG, KICH_THUOC_MANG + 1, &gia_tri);

    return 0;
}
