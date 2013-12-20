#ifndef QTUTIL_GRID3DVIEW_HPP
#define QTUTIL_GRID3DVIEW_HPP

#include <QSet>
#include <QWidget>

#include <memory>



namespace QtUtil
{
	class Grid3DView;
}



class QtUtil :: Grid3DView : public QWidget
{
Q_OBJECT

public:

	enum class Plane
	{
		XY,
		XZ,
		YZ
	};

	struct Position
	{
		Position ();

		Position (int x, int y, int z);

		int x;
		int y;
		int z;

		bool operator == (const Position &) const;
		bool operator != (const Position &) const;

		Position & operator += (const Position &);
		Position operator + (const Position &) const;
	};

	Grid3DView (QWidget * parent = nullptr);

	~ Grid3DView ();

	const Position & position () const;

	const Position & cursor () const;

	//! SharedState will be shared with mergee. Also connects signals/slots.
	void join (Grid3DView * mergee);

	Plane plane () const;

signals:

	void position_changed (int x, int y, int z);

	void cursor_changed (int x, int y, int z);

public slots:

	void set_plane (Plane);

	void set_width (int);

	void set_position (int x, int y, int z);

	virtual void set_cursor (int x, int y, int z);

	//! Sets the position to the cursor position.
	void to_cursor ();

	//! Sets the m component of the position to the cursor.
	void to_cursor_plane ();

protected:

	class SharedState;

	//! Implementation must allocate a new object.
	virtual SharedState * create_shared_state () const;

	//! The state shared between many Grid3DView which have been join()ed.
	SharedState & shared_state ();

	const SharedState & shared_state () const;

	//! Outputs i,j of pixel relative to central square.
	void window_to_ik (int x, int y, int * i, int * k) const;

	//! e.g. when plane is XY returns {i,j,0}
	Position to_global (int i, int k) const;

	//! A drag action was indicated from the cursor to cursor+delta.
	virtual void drag (const Position & delta);

	/**
	 * @details
	 *
	 * The painter is set up so that (0,0) is the bottom-left corner
	 * and (1,1) is the top-right corner. The implementation should
	 * paint the contents of the world at the given position using only
	 * this space. If you draw outside of this space, the effect of overlap
	 * on other tiles is undefined.
	 **/
	virtual void draw (QPainter &, const Position &) const = 0;

	//! Calls f(x) on each view x joined to this one (including this).
	template <typename F>
	void each_joined (const F &);

	//! Calls each_joined(update).
	void update_joined ();

private slots:

	void set_xy ();

	void set_xz ();

	void set_yz ();

private:

	std :: shared_ptr <SharedState> m_shared_state;

	Plane m_plane;

	//! Real width is 2*m_width+1
	unsigned int m_width;

	Position m_position;

	Position m_cursor;

	struct DragInfo
	{
		bool dragging;

		int di;
		int dk;

		DragInfo ();
	};

	DragInfo m_drag_info;

	//! x for XY or XZ planes, z for YZ plane.
	int get_i (const Position &) const;

	//! y for XY or YZ planes, z for XZ plane.
	int get_k (const Position &) const;

	//! z for XY plane, y for XZ plane, x for YZ plane
	int get_m (const Position &) const;

	void keyPressEvent (QKeyEvent *) override;

	void keyReleaseEvent (QKeyEvent *) override;

	void paintEvent (QPaintEvent *) override;

	void contextMenuEvent (QContextMenuEvent *) override;

	void enterEvent (QEvent *) override;

	void leaveEvent (QEvent *) override;

	void mousePressEvent (QMouseEvent *) override;

	void paint (QPainter &, int width, int height) const;

	void draw_cursor (QPainter &, int width, int height) const;

	void draw_axes (QPainter &, int width, int height) const;

	void move_i (int);

	void move_k (int);

	void move_m (int);

	static void sync (Grid3DView * a, Grid3DView * b);
};



class QtUtil :: Grid3DView :: SharedState
{
public:

	virtual ~ SharedState ();

private:

	friend class QtUtil :: Grid3DView;

	QSet <Grid3DView *> m_views;
};



template <typename F>
void
QtUtil :: Grid3DView :: each_joined (const F & f)
{
	if (m_shared_state)
	{
		for (auto v : m_shared_state -> m_views)
			f (v);
	}
	else
		f (this);
}



#endif
