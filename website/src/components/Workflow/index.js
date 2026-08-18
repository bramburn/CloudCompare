import clsx from 'clsx';
import Link from '@docusaurus/Link';
import Heading from '@theme/Heading';
import styles from './styles.module.css';

/**
 * Workflow section — a 2-column "what CloudCompare does" block that pairs
 * a hero illustration (workflow-comparison.webp) with the canonical
 * terrestrial-laser-scanning → cloud-to-cloud-distance → difference-map
 * pipeline. Kept narrow on purpose so the hero illustration and the prose
 * stay side by side even on a 996px breakpoint.
 */
function Workflow() {
  return (
    <section className={styles.workflow}>
      <div className="container">
        <div className={styles.workflowHeader}>
          <span className={styles.eyebrow}>The canonical workflow</span>
          <Heading as="h2">From raw scan to difference map</Heading>
          <p>
            Three CloudCompare steps cover most surveying-company jobs. Open
            the scans, align them, colour the deltas. The same shape, the
            same plugin set, on a phone-class Windows box.
          </p>
        </div>
        <div className={clsx('row', styles.workflowRow)}>
          <div className={clsx('col col--6', styles.workflowVisual)}>
            <div className={styles.workflowFigure}>
              <picture>
                <source
                  srcSet="/CloudCompare/img/workflow-comparison.webp"
                  type="image/webp"
                />
                <img
                  src="/CloudCompare/img/workflow-comparison.png"
                  alt="Two point clouds inside wireframe meshes connected by a red-blue distance heatmap"
                  width="1400"
                  height="781"
                  loading="lazy"
                />
              </picture>
              <div className={styles.workflowFigureCaption}>
                qM3C2 / Cloud-to-Cloud Distance — two clouds, one heatmap.
              </div>
            </div>
          </div>
          <div className={clsx('col col--6', styles.workflowSteps)}>
            <ol className={styles.workflowList}>
              <li>
                <div className={styles.workflowStepNum}>01</div>
                <div className={styles.workflowStepBody}>
                  <h3>Open the scans</h3>
                  <p>
                    Drop a <code>.las</code> / <code>.e57</code> / <code>.ply</code> from
                    the terrestrial or mobile laser scanner into the DB tree.
                    Octree-based streaming keeps 120M+ point clouds responsive
                    on a 2&nbsp;GB-RAM workstation.
                  </p>
                </div>
              </li>
              <li>
                <div className={styles.workflowStepNum}>02</div>
                <div className={styles.workflowStepBody}>
                  <h3>Align &amp; segment</h3>
                  <p>
                    RANSAC for planes, spheres, and cylinders. CSF to strip
                    ground points. qHPR to cull points invisible from the
                    scanner. qCanupo to classify at the point level.
                  </p>
                </div>
              </li>
              <li>
                <div className={styles.workflowStepNum}>03</div>
                <div className={styles.workflowStepBody}>
                  <h3>Compare &amp; export</h3>
                  <p>
                    M3C2 multiscale distance between the epoch and the
                    reference. Colour-map the deltas. Export to a{' '}
                    <code>.las</code> or <code>.ply</code> deliverable for the
                    client.
                  </p>
                </div>
              </li>
            </ol>
            <div className={styles.workflowCta}>
              <Link
                className="button button--secondary button--lg"
                to="/docs/plugins/standard">
                See all 18 plugins →
              </Link>
            </div>
          </div>
        </div>
      </div>
    </section>
  );
}

export default Workflow;
