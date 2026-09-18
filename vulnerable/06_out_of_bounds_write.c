/*
 * =============================================================================
 * Tên tệp       : oob_write_vuln.c
 * Kịch bản       : Out-of-bounds Write (CWE-787) - #1 CWE Top 25 (2023)
 * Mô tả         : Phiên bản CÓ LỖ HỔNG - ghi dữ liệu ngoài phạm vi mảng
 *
 * So với bản gốc, thay đổi:
 *   - Hàm cap_nhat_phan_tu() không kiểm tra biên -> ghi ngoài mảng
 *   - Có thể ghi đè biến khác trên stack, gây crash hoặc RCE
 *
 * Lỗ hổng:
 *   - CWE-787: Out-of-bounds Write
 *   - CWE-119: Improper Restriction of Operations within Buffer Bounds
 *   - CVE liên quan: CVE-2021-3156 (Sudo Baron Samedit), CVE-2019-0703 (WinRAR)
 *   - Rủi ro: ghi đè return address -> thực thi mã tùy ý
 *
 * Dùng trong demo: SCGL phải phát hiện commit này là BUGGY.
 * =============================================================================
 */

#include <stdio.h>
#include <string.h>

#define KICH_THUOC_MANG    10
#define GIA_TRI_MAC_DINH   42

typedef struct {
    int kich_thuoc_thuc_te;           /* Cờ kiểm soát - bị ghi đè nếu OOB */
    int mang_du_lieu[KICH_THUOC_MANG];
} MangDong;

void khoi_tao_mang_dong(MangDong *md) {
    if (md == NULL) return;
    md->kich_thuoc_thuc_te = KICH_THUOC_MANG;
    for (int i = 0; i < KICH_THUOC_MANG; i++) {
        md->mang_du_lieu[i] = i * 10;
    }
}

void in_mang_dong(const MangDong *md) {
    if (md == NULL) return;
    printf("[Kich thuoc] %d\n", md->kich_thuoc_thuc_te);
    printf("[Mang] ");
    for (int i = 0; i < md->kich_thuoc_thuc_te && i < KICH_THUOC_MANG; i++) {
        printf("%d ", md->mang_du_lieu[i]);
    }
    printf("\n");
}

/* HÀM NÀY CHỨA LỖ HỔNG OUT-OF-BOUNDS WRITE */
int cap_nhat_phan_tu(MangDong *md, int vi_tri, int gia_tri_moi) {
    if (md == NULL) return -1;
    /* LỖI: khong kiem tra vi_tri co nam trong mang hay khong! */
    md->mang_du_lieu[vi_tri] = gia_tri_moi;  /* Có thể ghi ngoài phạm vi */
    return 0;
}

int main(int argc, char **argv) {
    MangDong md;
    khoi_tao_mang_dong(&md);
    in_mang_dong(&md);

    /* Demo: ghi vào vị trí hợp lệ */
    cap_nhat_phan_tu(&md, 2, 999);
    in_mang_dong(&md);

    /* Demo: ghi NGOÀI phạm vi - ghi đè kich_thuoc_thuc_te! */
    printf("[CANH BAO] Loi hong OOB Write tai vi_tri=%d\n", -1);
    cap_nhat_phan_tu(&md, -1, 99999);  /* Ghi đè kich_thuoc_thuc_te */
    in_mang_dong(&md);

    return 0;
}
