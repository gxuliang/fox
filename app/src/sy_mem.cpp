






packet* getpacket(int sn)
{
	static int i[2]={0,0};
	packet *pkt = videopkt[sn][i[sn]];
	trd_lock(&pkt->lock);
	if(pkt->used == 0)
	{
		trd_unlock(&pkt->lock);
		return null;
	}
	printf("%d,%d,%d\n", pkt->u32Seq, pkt->pts, pkt->frame);
	i[sn]++;
	if(i[sn] >= MAX_FPS)
	{
		i[sn] = 0;
	}
	trd_unlock(&pkt->lock);

	return pkt;
}

int releasepacket(packet* pkt)
{
	if(pkt == NULL)
		return -1;

	trd_lock(&pkt->lock);
	pkt->used = 0;
	trd_unlock(&pkt->lock);

	return 1;
}