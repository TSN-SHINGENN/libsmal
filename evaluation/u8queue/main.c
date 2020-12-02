#include <stdio.h>

#include "smal_malloc.h"
#include "smal_u8queue.h"

/*!
 * @brief     �z��v�f�ʒu���҂��s��͈͂ł��邩�m�F����
 * @param[in] q   �L���[�Ǘ�
 * @param[in] id  �z��̗v�f�ʒu
 */
static int
is_use_range(const smal_u8queue_t *const q, unsigned int id)
{
    if((id < q->tail) && (id >= q->head)) {
	return 1;
    }
    if((id > q->tail) && (id >= q->head) && (q->head > q->tail)) {
	return 1;
    }
    if((id < q->tail) && (id < q->head)  && (q->head > q->tail)) {
	return 1;
    }

    return 0;
}

/*!
 * @brief      �L���[�̗v�f���ꗗ�\������
 * @param[in]  q     �L���[�Ǘ�
 */
static void
queue_print(const smal_u8queue_t *const q)
{
    unsigned int cnt;

    printf("queue [");
    for(cnt = 0; cnt < q->queue_size; cnt++){
        if(is_use_range(q, cnt)){
            printf("%2d", q->buf[cnt]); 
        }else{
            printf("%2c", '.');
        }
    }
    printf("]\n");
}

int
main(void)
{
    smal_u8queue_t q;
    uint8_t data = 0;
    int cnt;
    char VHEAP[128];

    smal_malloc_init(VHEAP, 128);

    smal_u8queue_init(&q, 20);

    /* �L���[�Ƀf�[�^����͂��� */
    for(cnt = 1; cnt < 5; cnt++){
        printf("enqueue %d : ", cnt);
        if(smal_u8queue_push(&q, cnt) != 0){
            printf("Queue is full.\n");
        }else{
            queue_print(&q);
        }
    }

    /* �L���[����f�[�^���擾���� */
    for(cnt = 1; cnt < 4; cnt++){
        if(smal_u8queue_pop(&q, &data) != 0){
            printf("Queue is empty.\n");
        }else{
            printf("dequeue %d : ", data);
            queue_print(&q);
        }
    }

    /* �L���[�Ƀf�[�^����͂��� */
    for(cnt = 5; cnt < 10; cnt++){
        printf("enqueue %d : ", cnt);
        if(smal_u8queue_push(&q, cnt) != 0){
            printf("Queue is full.\n");
        }else{
            queue_print(&q);
        }
    }

    /* �L���[����f�[�^���擾���� */
    for(cnt = 1; cnt < 4; cnt++){
        if(smal_u8queue_pop(&q, &data) != 0){
            printf("Queue is empty.\n");
        }else{
            printf("dequeue %d : ", data);
            queue_print(&q);
        }
    }

    return 0;
}
